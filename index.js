

var addon = require('./build/Release/imgnetfree');

module.exports = addon;

var fs = require('fs');

module.exports.addImageTitle(fs.readFileSync(__dirname + '/checking.png'), 1);
module.exports.addImageTitle(fs.readFileSync(__dirname + '/overflow.png'), 2);
module.exports.addImageTitle(fs.readFileSync(__dirname + '/block.png'), 3);

module.exports.formats = {
	FIF_BMP		: 0,
	FIF_ICO		: 1,
	FIF_JPEG	: 2,
	FIF_JNG		: 3,
	FIF_KOALA	: 4,
	FIF_LBM		: 5,
	FIF_IFF		: 5,
	FIF_MNG		: 6,
	FIF_PBM		: 7,
	FIF_PBMRAW	: 8,
	FIF_PCD		: 9,
	FIF_PCX		: 10,
	FIF_PGM		: 11,
	FIF_PGMRAW	: 12,
	FIF_PNG		: 13,
	FIF_PPM		: 14,
	FIF_PPMRAW	: 15,
	FIF_RAS		: 16,
	FIF_TARGA	: 17,
	FIF_TIFF	: 18,
	FIF_WBMP	: 19,
	FIF_PSD		: 20,
	FIF_CUT		: 21,
	FIF_XBM		: 22,
	FIF_XPM		: 23,
	FIF_DDS		: 24,
	FIF_GIF		: 25,
	FIF_HDR		: 26,
	FIF_FAXG3	: 27,
	FIF_SGI		: 28,
	FIF_EXR		: 29,
	FIF_J2K		: 30,
	FIF_JP2		: 31,
	FIF_PFM		: 32,
	FIF_PICT	: 33,
	FIF_RAW		: 34,
	FIF_WEBP	: 35,
	FIF_JXR		: 36
};