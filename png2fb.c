/*
 * PNG to 565 16-bit converter with a set of [useful] options
 * May be useful in TV-sets or IPTV/hybrid STBs for preparing framebuffer splashes
 * Copyright P.Krasnov<pkrasnov@yandex.ru>
 * License GPL v3
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<getopt.h>
#include<stdint.h>
#include<png.h>
#include <sys/types.h>
#include <fcntl.h>

void usage(char*);

int main(int argc, char**argv)
{
  int c;
  int digit_optind = 0;
  int verbose=0;
  int this_option_optind = optind ? optind : 1;
  int option_index = 0;
  static char*input;
  static char*output;
  static int owidth;
  static int oheight;
  static unsigned int color;
  static int valign;
  static int halign;
  FILE*fp;
  int out;
  uint8_t header[8];
  int is_png;
  png_structp png_ptr;
  png_infop info_ptr;
  png_uint_32 iwidth, iheight;
  png_bytepp row_pointers;
  int bit_depth, color_type;
  int i,ii,j;
  uint16_t bg;
  int width,height;
  uint16_t**buf;
  int kh,kw;
  uint32_t red,green,blue;
  /*
 struct option {
               const char *name;
               int         has_arg;
               int        *flag;
               int         val;
           };
 */
    static struct option long_options[]=
    {
      {"help",		no_argument,	0,	'h'},
      {"usage",		no_argument,	0,	'u'},
      {"verbose",	no_argument,	0,	'v'},
      {"input",		required_argument,	0,	'i'},
      {"output",	required_argument,	0,	'o'},
      {"width",		required_argument, 0,  0},
      {"height",	required_argument, 0,  0},
      {"valign",	required_argument, 0,  0},
      {"halign",	required_argument, 0,  0},
      {"color",		required_argument, 0,  'c'},
      {0,0,0,0}
    };
    if(1==argc)usage(argv[0]);
  while (1) {
    c = getopt_long(argc, argv, "huvi:o:c:W;",long_options, &option_index);
    if (c == -1)break;
      switch (c) {
      case 0:
		   if(strcmp(long_options[option_index].name, "input")==0)
		   {
		     input=optarg;
		     break;
		   }
		   else if(strcmp(long_options[option_index].name, "output")==0)
		   {
		     output=optarg;
		     break;
		   }
		   else if(strcmp(long_options[option_index].name, "width")==0)
		   {
		     owidth=strtol(optarg, NULL, 10);
		     break;
		   }
		   else if(strcmp(long_options[option_index].name, "height")==0)
		   {
		     oheight=strtol(optarg, NULL, 10);
		     break;
		   }
		   else if(strcmp(long_options[option_index].name, "valign")==0)//top|center|bottom
		   {
		     if(strcmp(optarg, "center")==0)valign=1;
		     else if (strcmp(optarg, "bottom")==0)valign=2;
		     else valign=0;
		     break;
		   }
		   else if(strcmp(long_options[option_index].name, "halign")==0)
		   {
		     if(strcmp(optarg, "center")==0)halign=1;
		     else if (strcmp(optarg, "right")==0)halign=2;
		     else halign=0;
		     break;
		   }
		   else if(strcmp(long_options[option_index].name, "color")==0)
		   {
		     color=strtol(optarg, NULL, 16);
		     break;
		   }
		   break;
      case '1':
		   break;
      case '2':
                   if (digit_optind != 0 && digit_optind != this_option_optind)
                     printf("digits occur in two different argv-elements.\n");
                   digit_optind = this_option_optind;
                   printf("option %c\n", c);
                   break;
      case 'h':
                   usage(argv[0]);
                   break;
      case 'v':
		   verbose=1;
                   break;
      case 'i':
		   if(optarg)input=optarg;
		   break;
      case 'o':
		   if(optarg)output=optarg;
		   break;
      case 'c':
		   color=strtol(optarg, NULL, 16);
		   break;
      case '?':
                   break;
      default:
                   printf("Option character code 0%o ?\n", c);
               }
           }

           if (optind < argc) {
               printf("non-option ARGV-elements: ");
               while (optind < argc)
                   printf("%s ", argv[optind++]);
               printf("\tignored\n");
           }
  if(!input)usage(argv[0]);
  if(!((owidth&&oheight)||(!owidth&&!oheight)))usage(argv[0]);
  output?(out=open(output,O_WRONLY|O_CREAT,0666)):(out=0);
  if(out<0)
  {
    perror("Destination file");
    return 1;
  }
  fp=fopen(input, "r");
  if(!fp)
  {
   perror("Source PNG:");
   return 1;
  }
  fread(header, 1, 8, fp);
  is_png=!png_sig_cmp(header, 0, 8);
  if (!is_png)
  {
   fprintf(stderr,"Invalid PNG\n"); 
   return 1;
  }
  png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,(png_voidp)NULL,NULL,NULL);
  if (!png_ptr)
  {
   fprintf(stderr,"Error creating png read struct");
   return 1;
  }
  info_ptr=png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr,(png_infopp)NULL, (png_infopp)NULL);
    fprintf(stderr,"Error creating png info struct");
    return 1;
  }
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    fprintf(stderr,"libpng setjmp error");
    png_destroy_read_struct(&png_ptr, &info_ptr,NULL);
    fclose(fp);
    return 1;
  }
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info (png_ptr, info_ptr);	/* read all PNG info up to image data */
  png_get_IHDR (png_ptr, info_ptr, &iwidth, &iheight, &bit_depth, &color_type, NULL, NULL, NULL);
  if(verbose)
  {
    fprintf(stderr,"PNG image %s width %d height %d color type %d. ",input, (int)iwidth, (int)iheight, color_type);
    output?(fprintf(stderr,"Output: %s\n",output)):(fprintf(stderr,"Output: stdout\n"));
  }
  if(color_type==PNG_COLOR_TYPE_PALETTE)
  {
    png_set_palette_to_rgb(png_ptr);
  }
  if(color_type==PNG_COLOR_TYPE_GRAY||color_type==PNG_COLOR_TYPE_GRAY_ALPHA)
  {
   if(bit_depth<8)png_set_gray_1_2_4_to_8(png_ptr);
   png_set_gray_to_rgb(png_ptr);
  }
  if(bit_depth<8)
  {
    png_set_packing(png_ptr);
  }
  if(color_type & PNG_COLOR_MASK_ALPHA)
  {  
    png_set_strip_alpha(png_ptr);
  }
  png_read_update_info(png_ptr, info_ptr);
  row_pointers=png_malloc(png_ptr, iheight*png_sizeof(png_bytep));
  if(!row_pointers)
  {
   perror("PNG memory");
   return 1;
  }
  for (i=0; i<iheight; i++)row_pointers[i]=NULL;  /* security precaution */
  for (i=0; i<iheight;i++)row_pointers[i]=png_malloc(png_ptr,iwidth*8);//pixel size
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_read_image(png_ptr, row_pointers);

  blue=(color<<24)>>27;
  green=(color<<16)>>26;
  red=(color<<8)>>27;
  bg=blue|green<<5|red<<11;

  oheight?(height=oheight):(height=iheight);
  owidth?(width=owidth):(width=iwidth);
  buf=(uint16_t**)malloc(height*sizeof(uint16_t*));
  if(!buf)
  {
    perror("Memory");
    return 1;
  }
  for(i=0;i<height;i++)
  {
    buf[i]=(uint16_t*)malloc(width*sizeof(uint16_t));
    if(!buf[i])
    {
      perror("Memory");
      return 1;
    }
    if((iheight<height)||(iwidth<width))
    {
      for(ii=0;ii<width;ii++)buf[i][ii]=bg;
    }
  }
  kh=abs(oheight-iheight)/2*valign;
  kw=abs(owidth-iwidth)/2*halign;
  if(iheight<oheight)
  {
    for(i=kh;i<kh+iheight;i++)
    {
      if(iwidth>=owidth)
      {
	for(ii=0,j=kw*3;ii<width;ii++)
	{
	  buf[i][ii]=((((row_pointers[i-kh][j])>>3)<<11)|(((row_pointers[i-kh][j+1])>>2)<<5)|((row_pointers[i-kh][j+2])>>3));
	  j+=3;
	}
      }
      else
      {
	for(ii=kw,j=0;ii<(kw+iwidth);ii++)
	{
	  buf[i][ii]=((((row_pointers[i-kh][j])>>3)<<11)|(((row_pointers[i-kh][j+1])>>2)<<5)|((row_pointers[i-kh][j+2])>>3));
	  j+=3;
	}
      }
    }
  }
  else
  {
    for(i=0;i<height;i++)
    {
      if(iwidth>=owidth)
      {
	for(ii=0,j=kw*3;ii<width;ii++)
	{
	  buf[i][ii]=((((row_pointers[i+kh][j])>>3)<<11)|(((row_pointers[i+kh][j+1])>>2)<<5)|((row_pointers[i+kh][j+2])>>3));
	  j+=3;
	}
      }
      else
      {
	for(ii=kw,j=0;ii<(kw+iwidth);ii++)
	{
	  buf[i][ii]=((((row_pointers[i+kh][j])>>3)<<11)|(((row_pointers[i+kh][j+1])>>2)<<5)|((row_pointers[i+kh][j+2])>>3));
	  j+=3;
	}
      }
    }
  }
  
  for(i=0;i<height;i++)
  {
    write(out,buf[i],2*width);
  }
  
  png_destroy_read_struct(&png_ptr, &info_ptr,(png_infopp)NULL);
  fclose(fp);
  if(output)close(out);
  for(i=0;i<height;i++)
  {
    free(buf[i]);
  }
  free(buf);
  return 0;
}

void usage(char*progname)
{
    printf("Usage:\n%s\n",progname);
    printf("--help, -h, --usage\tshort description\n");
    printf("--verbose, -v\t\tbe verbose\n");
    printf("--input FILE\t\tsource PNG image file. Required parameter\n");
    printf("--output FILE\t\tdestination file. Default is stdout\n");
    printf("--width NUMBER\t\twidth\n");
    printf("--height NUMBER\t\tand height of output\n");
    printf("--valign RULE\t\tvertical alignment [top|center|bottom]. Top is default\n");
    printf("--halign RULE\t\thorisontal alignment [left|center|right]. Left is default\n");
    printf("--color HEX\t\tbackground color in HEX RGB like 0xFF0000 or ff0000 for red. Default 0 (black)\n");
    exit(0);
}
