/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "picture.h"
#include <stdio.h>
#include <string.h>

#define MAXWIDTH 1024
#define MAXHEIGHT 1024

#ifdef _WIN32
#include <d3dx8.h>
#include "m3d_priv.h"

extern LPDIRECT3DDEVICE8 render_device( render *r );

static char *jpeg_read( void *data, int size, int *twidth, int *theight ) {
	D3DXIMAGE_INFO inf;
	if( D3DXGetImageInfoFromFileInMemory(data,size,&inf) != D3D_OK )
		return NULL;
	if( inf.ImageFileFormat != D3DXIFF_JPG || inf.Width < 0 || inf.Height < 0 || inf.Width > MAXWIDTH || inf.Height > MAXHEIGHT )
		return NULL;
	LPDIRECT3DTEXTURE8 tex;
	if( D3DXCreateTextureFromFileInMemoryEx( render_device(M3D()->render), data, size, inf.Width, inf.Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &tex ) != D3D_OK ) {
		tex->Release();
		return NULL;
	}
	D3DLOCKED_RECT lock;
	if( tex->LockRect(0,&lock,NULL,0) ) {
		tex->Release();
		return NULL;
	}
	DWORD *buf = new DWORD[inf.Width * inf.Height];
	unsigned int y;
	for(y=0;y<inf.Height;y++)
		memcpy(buf + y * inf.Width, (DWORD*)lock.pBits + y * lock.Pitch, inf.Width * 4);
	tex->UnlockRect(0);
	tex->Release();
	return (char*)buf;
}

#else 

extern "C" {
#include "../../libs/jpeg/jpeglib.h"
};

static void jpg_init_source(j_decompress_ptr cinfo) {
}

static boolean jpg_fill_input_buffer( j_decompress_ptr cinfo ) {
	static JOCTET eof[] = { (JOCTET) 0xFF, (JOCTET) JPEG_EOI };
	cinfo->src->next_input_byte = eof;
	cinfo->src->bytes_in_buffer = 2;
	return TRUE;
}

static void jpg_skip_input_data( j_decompress_ptr cinfo, long num_bytes ) {
	cinfo->src->next_input_byte += num_bytes;
	if( cinfo->src->bytes_in_buffer < (unsigned int)num_bytes )
		cinfo->src->bytes_in_buffer = 0;
	else
		cinfo->src->bytes_in_buffer -= num_bytes;
}

static void jpg_term_source( j_decompress_ptr cinfo ) {
}

static void jpg_error_exit( j_common_ptr cinfo ) {
	throw("jpeg error");
}

static char *jpeg_read( void *data, int size, int *twidth, int *theight ) {
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	cinfo.err->error_exit = jpg_error_exit;
	jpeg_create_decompress(&cinfo);
	cinfo.src = new jpeg_source_mgr;
	cinfo.src->next_input_byte = (JOCTET*)data;
	cinfo.src->bytes_in_buffer = size;
	cinfo.src->init_source = jpg_init_source;
	cinfo.src->fill_input_buffer = jpg_fill_input_buffer;
	cinfo.src->skip_input_data = jpg_skip_input_data;
	cinfo.src->resync_to_restart = jpeg_resync_to_restart; // use default method
	cinfo.src->term_source = jpg_term_source;

	try {
		jpeg_read_header(&cinfo, TRUE);
	} catch( const char * ) {
	}
	*twidth = cinfo.image_width;
	*theight = cinfo.image_height;
	if( cinfo.image_width <= 0 || cinfo.image_height <= 0 || cinfo.image_width > MAXWIDTH || cinfo.image_height > MAXHEIGHT || cinfo.out_color_space != JCS_RGB ) {
		jpeg_destroy_decompress(&cinfo);
		delete cinfo.src;
		return NULL;
	}
	
	jpeg_start_decompress(&cinfo);
	int w = (int)cinfo.image_width;
	unsigned char *buffer = new unsigned char[cinfo.image_width * cinfo.image_height * 4];	
	unsigned char *buf = buffer;
	try {
		while( cinfo.output_scanline < cinfo.output_height ) {
			int l = cinfo.output_scanline;
			jpeg_read_scanlines(&cinfo,&buf,1);
			if( cinfo.output_scanline != l + 1 )
				break;
			int i;
			unsigned char *buf2 = buf + 4 * w;
			buf += 3 * w;
			for(i=0;i<w;i++) {
				buf-=3;
				buf2-=4;
				buf2[0] = buf[2];
				buf2[1] = buf[1];
				buf2[2] = buf[0];
				buf2[3] = 0xFF;
			}
			buf += 4 * w;
		}
	} catch( const char * ) {
	}
 	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	delete cinfo.src;
	return (char*)buffer;
}

#endif

static char *raw_read( void *data, int size, int *twidth, int *theight ) {
	if( size < 8 )
		return NULL;
	int w = ((int*)data)[0];
	int h = ((int*)data)[1];
	if( w < 0 || h < 0 || w > MAXWIDTH || h > MAXHEIGHT || size != 8 + w * h * 4 )
		return NULL;
	*twidth = w;
	*theight = h;
	char *raw = new char[w*h*4];
	memcpy(raw,((char*)data) + 8, size - 8);
	return raw;
}

char *picture_read( void *data, int size, pic_kind kind, int *twidth, int *theight ) {
	switch( kind ) {
	case PIC_JPEG:
		return jpeg_read(data,size,twidth,theight);
	case PIC_RAW:
		return raw_read(data,size,twidth,theight);
	default:
		return NULL;
	}
}

/* ************************************************************************ */
