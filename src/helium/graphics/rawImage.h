/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_RAWIMAGE
#define HE_RAWIMAGE

#include <helium/vector/array.h>

namespace helium{
  //assumes data is RGB
  class RawImage {
  protected:
    int imageWidth,imageHeight;
  private:
    unsigned char *imageData;
  public:
    RawImage():imageData(NULL){}
    virtual ~RawImage(){
      if (imageData!=NULL){
	delete[] imageData;
      }
    }
    
    void resize(int w,int h){
      if (imageData!=NULL){
	delete imageData;
      }
      imageData=new unsigned char[w*h*3];      
      imageWidth=w;
      imageHeight=h;
    }

    int width() { return imageWidth; }
    int height() { return imageHeight; }
    unsigned char *data() { return imageData; }
    

    unsigned char* operator()(int r,int c){
      return imageData+3*(r*imageWidth+c);
    }

  };
}


#endif
