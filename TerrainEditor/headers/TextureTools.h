/* 
 * File:   TextureTools.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 21:16
 */

#ifndef TEXTURETOOLS_H
#define	TEXTURETOOLS_H

#include <IImage.h>
#include <ITexture.h>
#include <IVideoDriver.h>

namespace irr {

    /**
     * Class for some useful texture tools such converters & previews creators
     */
    class TextureTools {
    public:
        /**
         * Constructor
         * 
         * @param pVideoDriver  pointer to IVideoDriver interface
         */
        TextureTools(video::IVideoDriver* pVideoDriver);

        /**
         * Copy constructor (not implemented)
         * 
         * @param orig  reference to original instance of this class
         */
        TextureTools(const TextureTools& orig);

        /**
         * Virtual destructor (not implemented)
         */
        virtual ~TextureTools();

        /**
         * Converter from ITexture* to IImage*
         *
         * @param pTexture	ITexture pointer to be converted
         * @return			pointer to IImage
         */
        video::IImage* TextureToImage(video::ITexture* pTexture);

        /**
         * Converter from IImage* to ITexture*
         *
         * @param pImage		IImage pointer to be converted
         * @param textureName		name of result ITexture*
         * @return			pointer to ITexture
         */
        video::ITexture* ImageToTexture(video::IImage* pImage, core::stringc textureName);

        /**
         * Creates square texture preview from first 32 pixes on both dimensions of original texture
         * 
         * @param originalTexture       pointer to ITexture interface of original texture
         * @return pointer to ITexture interface of preview texture
         */
        video::ITexture* createTexturePreview32(video::ITexture* originalTexture);

        /**
         * Creates square texture preview from first 64 pixes on both dimensions of original texture
         * 
         * @param originalTexture       pointer to ITexture interface of original texture
         * @return pointer to ITexture interface of preview texture
         */
        video::ITexture* createTexturePreview64(video::ITexture* originalTexture);

        /**
         * Creates square texture preview from first 128 pixes on both dimensions of original texture
         * 
         * @param originalTexture       pointer to ITexture interface of original texture
         * @return pointer to ITexture interface of preview texture
         */
        video::ITexture* createTexturePreview128(video::ITexture* originalTexture);

        /**
         * Creates square texture preview from first 256 pixes on both dimensions of original texture
         * 
         * @param originalTexture       pointer to ITexture interface of original texture
         * @return pointer to ITexture interface of preview texture
         */
        video::ITexture* createTexturePreview256(video::ITexture* originalTexture);
    private:
        /**
         * Creates square texture preview from first pixes on both dimensions of original texture
         * Usefull for texture editor/manager previews or some selection
         * 
         * @param originalTexture       pointer to ITexture interface of original texture
         * @param textureDimension      preview texture dimension on both dimensions
         * @return pointer to ITexture interface of preview texture
         */
        video::ITexture* createTexturePreview(video::ITexture* originalTexture, u32 textureDimension);

        /**
         * Pointer to IVideoDriver interface
         */
        video::IVideoDriver* pVideoDriver;
    };
} // end of namespace irr

#endif	/* TEXTURETOOLS_H */

