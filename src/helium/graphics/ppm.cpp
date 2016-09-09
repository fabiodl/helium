/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#include <helium/graphics/ppm.h>
#include <helium/core/exception.h>
#include <helium/util/ioUtil.h>
#include <stdio.h>

//Much of this code comes from Open Dynamics Engine, 
//Copyright (C) 2001-2003 Russell L. Smith. 
//drawstuff.cpp



using namespace helium;


static void skipWhiteSpace (const char *filename, FILE *f)
{
  int c,d;
  for(;;) {
    c = fgetc(f);
    if (c==EOF) 
      throw exc::BadFileFormat("PPM",filename,"unexpected end of file");
    if (c == '#') {// skip comments
      do {
	d = fgetc(f);
	if (d==EOF)
	  throw exc::BadFileFormat("PPM",filename,"unexpected end of file");
      } while (d != '\n');
      continue;
    }
    if (c > ' ') {
      ungetc (c,f);
      return;
    }
  }
}


// read a number from a stream, this return 0 if there is none (that's okay
// because 0 is a bad value for all PPM numbers anyway).

static int readNumber (const char *filename, FILE *f)
{
  int c,n=0;
  for(;;) {
    c = fgetc(f);
    if (c==EOF) 
      	  throw exc::BadFileFormat("PPM",filename,"unexpected end of file");
    if (c >= '0' && c <= '9') n = n*10 + (c - '0');
    else {
      ungetc (c,f);
      return n;
    }
  }
}


namespace helium{

  void loadppm(RawImage& r,const char* filename){
    int image_width,image_height;
    FILE *f = fopen (filename,"rb");
    if (!f){
      throw exc::FileOpenRead(filename);
    }

    if (fgetc(f) != 'P' || fgetc(f) != '6'){
      throw exc::BadFileFormat("PPM",filename);
    }

    skipWhiteSpace (filename,f);
    image_width = readNumber (filename,f);
    skipWhiteSpace (filename,f);
    image_height = readNumber (filename,f);
    skipWhiteSpace (filename,f);
    int max_value = readNumber (filename,f);
    if (image_width < 1) {
      throw exc::BadFileFormat("PPM",filename,toString(image_width)+" is a bad width");
    }
    if (image_height < 1){
       throw exc::BadFileFormat("PPM",filename,toString(image_width)+" is a bad height");
    }
    if (max_value != 255){
      throw exc::BadFileFormat("PPM",filename,"must have color range of 255");      
    }
    int c = fgetc(f);
    if (c == 10) {
      // LF
    }
    else if (c == 13) {
      // CR
      c = fgetc(f);
      if (c != 10) ungetc (c,f);
    }
    else ungetc (c,f);
    r.resize(image_width,image_height);
    if (fread (r.data(),image_width*image_height*3,1,f) != 1){
      throw exc::BadFileFormat("PPM",filename,"error while reading data");
    }
      fclose (f);
  }

  void writeppm(const char* filename,int w,int h,const unsigned char* buff){
    //int i, j;
    FILE *fp = fopen(filename, "wb"); /* b - binary mode */
    if (!fp){
      throw exc::FileOpenWrite(filename);
    }

    fprintf(fp, "P6\n%d %d\n255\n", w, h);
    //int c;
    //std::cout<<w<<" x "<<h<<std::endl;
    fwrite(buff,3,w*h,fp);
    fclose(fp);        
  }
};


  
