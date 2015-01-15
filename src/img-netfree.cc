
#include <node.h>
#include <v8.h>

#include <node_buffer.h>
#include <node_object_wrap.h>

#include <FreeImage.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <math.h>


using namespace node;
using namespace v8;


struct Baton {
	uv_work_t request;
	Persistent<Function> callback;

	FIMEMORY* fiMemoryOut;
	FIMEMORY* fiMemoryIn;
	FIBITMAP* bitmap;
	u_int8_t* bits;
	u_int16_t sum;
};

static void imageBlurWork(uv_work_t* req) {

	Baton* baton = static_cast<Baton*>(req->data);
	//uint i;
	//ImageThumb* obj = baton->obj;

	FIMEMORY * fiMemoryIn = NULL;
	FIMEMORY * fiMemoryOut = NULL;
	FIBITMAP * fiBitmap = NULL, *thumbnail1 = NULL, *thumbnail2 = NULL;
	int width , height;
	fiMemoryIn = baton->fiMemoryIn;	//FreeImage_OpenMemory((BYTE *)baton->imageBuffer,baton->imageBufferLength);




	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(fiMemoryIn);



	if (format < 0 || FIF_GIF == format)
		goto ret;

	fiBitmap = FreeImage_LoadFromMemory(format, fiMemoryIn);

	if (!fiBitmap)
		goto ret;

//	FILTER_BOX		  = 0,	// Box, pulse, Fourier window, 1st order (constant) b-spline
//	FILTER_BICUBIC	  = 1,	// Mitchell & Netravali's two-param cubic filter
//	FILTER_BILINEAR   = 2,	// Bilinear filter
//	FILTER_BSPLINE	*  = 3,	// 4th order (cubic) b-spline
//	FILTER_CATMULLROM = 4,	// Catmull-Rom spline, Overhauser spline
//	FILTER_LANCZOS3	  = 5	// Lanczos3 filter


	width = FreeImage_GetWidth(fiBitmap);
	height = FreeImage_GetHeight(fiBitmap);

	thumbnail1 = FreeImage_Rescale(fiBitmap, 3, 3, FILTER_BOX);

	if (!thumbnail1)
		goto ret;

	thumbnail2 = FreeImage_Rescale( thumbnail1, width, height, FILTER_BOX );

	if (!thumbnail2)
		goto ret;

	fiMemoryOut = FreeImage_OpenMemory();

	FreeImage_SaveToMemory( FIF_PNG, thumbnail2, fiMemoryOut, 0 );

	ret:

	if (fiMemoryIn)
		FreeImage_CloseMemory(fiMemoryIn);
	if (fiBitmap)
		FreeImage_Unload(fiBitmap);
	if (thumbnail1)
		FreeImage_Unload(thumbnail1);
	if(thumbnail2)
		FreeImage_Unload( thumbnail2 );

	baton->fiMemoryOut =  fiMemoryOut;

}

static void imageBlurAfter(uv_work_t* req) {
	HandleScope scope;
	Baton* baton = static_cast<Baton*>(req->data);

	if ( baton->fiMemoryOut ) {
		const unsigned argc = 2;
		const char*data;
		int datalen;
		FreeImage_AcquireMemory(baton->fiMemoryOut,(BYTE**)&data, (DWORD*)&datalen );
		Local<Value> argv[argc] = {
			Local<Value>::New( Null() ),
			Local<Object>::New( Buffer::New((const char*)data,datalen)->handle_)
		};

		TryCatch try_catch;
		baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught())
			FatalException(try_catch);
	} else {
		Local < Value > err = Exception::Error(String::New("error"));

		const unsigned argc = 1;
		Local<Value> argv[argc] = {err};

		TryCatch try_catch;
		baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught())
			FatalException(try_catch);
	}

	if (baton->fiMemoryOut)
		FreeImage_CloseMemory(baton->fiMemoryOut);


	baton->callback.Dispose();
	delete baton;
}

static Handle<Value> imageBlur(const Arguments& args) {
	HandleScope scope;


	if (args.Length() < 2)
		return ThrowException(Exception::TypeError(String::New("Expecting 2 arguments")));

	if (!Buffer::HasInstance(args[0]))
		return ThrowException( Exception::TypeError( String::New("First argument must be a Buffer")));

	if (!args[1]->IsFunction())
		return ThrowException( Exception::TypeError( String::New( "Second argument must be a function")));


	Local < Function > callback = Local < Function > ::Cast(args[1]);

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION < 10
	Local < Object > buffer_obj = args[0]->ToObject();
#else
	Local<Value> buffer_obj = args[0];
#endif

	Baton* baton = new Baton();
	baton->request.data = baton;
	baton->callback = Persistent < Function > ::New(callback);


	baton->fiMemoryIn = FreeImage_OpenMemory((BYTE *) Buffer::Data(buffer_obj),
			Buffer::Length(buffer_obj));
	baton->fiMemoryOut = NULL;
	baton->bitmap = NULL;
	baton->bits = NULL;
	baton->sum = 0;

	int status = uv_queue_work(uv_default_loop(), &baton->request,
			imageBlurWork, (uv_after_work_cb) imageBlurAfter);

	assert(status == 0);
	return Undefined();
}



extern "C" {
	void init(Handle<Object> target) {
		HandleScope scope;

		target->Set(String::NewSymbol("imageBlur"), FunctionTemplate::New(imageBlur)->GetFunction());

	}

	NODE_MODULE(imgnetfree, init);
}
