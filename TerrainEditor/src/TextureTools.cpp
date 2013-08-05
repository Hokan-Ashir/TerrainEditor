/* 
 * File:   TextureTools.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 21:16
 */

#include "../headers/TextureTools.h"

namespace irr {

    /**
     * Constructor
     * 
     * @param pVideoDriver  pointer to IVideoDriver interface
     */
    TextureTools::TextureTools(video::IVideoDriver* pVideoDriver) {
        this->pVideoDriver = pVideoDriver;
    }

    /**
     * Copy constructor (not implemented)
     * 
     * @param orig  reference to original instance of this class
     */
    TextureTools::TextureTools(const TextureTools& orig) {
    }

    /**
     * Virtual destructor (not implemented)
     */
    TextureTools::~TextureTools() {
    }

    /**
     * Converter from ITexture* to IImage*
     *
     * @param pTexture	ITexture pointer to be converted
     * @return			pointer to IImage
     */
    video::IImage* TextureTools::TextureToImage(video::ITexture* pTexture) {
        video::IImage* pImage = pVideoDriver->createImageFromData(pTexture->getColorFormat(),
                pTexture->getSize(),
                pTexture->lock(),
                false); //copy memory

        pTexture->unlock();
        return pImage;
    }

    /**
     * Converter from IImage* to ITexture*
     *
     * @param pImage		IImage pointer to be converted
     * @param textureName		name of result ITexture*
     * @return			pointer to ITexture
     */
    video::ITexture* TextureTools::ImageToTexture(video::IImage* pImage, core::stringc textureName) {
        video::ITexture* pTexture = pVideoDriver->addTexture(textureName.c_str(), pImage);
        pTexture->grab();
        return pTexture;
    }

    /**
     * Creates square texture preview from first 32 pixes on both dimensions of original texture
     * 
     * @param originalTexture       pointer to ITexture interface of original texture
     * @return pointer to ITexture interface of preview texture
     */
    video::ITexture* TextureTools::createTexturePreview32(video::ITexture* originalTexture) {
        if (originalTexture->getSize().Height < 32) {
            return 0;
        } else {
            return createTexturePreview(originalTexture, 32);
        }
    }

    /**
     * Creates square texture preview from first 64 pixes on both dimensions of original texture
     * 
     * @param originalTexture       pointer to ITexture interface of original texture
     * @return pointer to ITexture interface of preview texture
     */
    video::ITexture* TextureTools::createTexturePreview64(video::ITexture* originalTexture) {
        if (originalTexture->getSize().Height < 64) {
            return 0;
        } else {
            return createTexturePreview(originalTexture, 64);
        }
    }

    /**
     * Creates square texture preview from first 128 pixes on both dimensions of original texture
     * 
     * @param originalTexture       pointer to ITexture interface of original texture
     * @return pointer to ITexture interface of preview texture
     */
    video::ITexture* TextureTools::createTexturePreview128(video::ITexture* originalTexture) {
        if (originalTexture->getSize().Height < 128) {
            return 0;
        } else {
            return createTexturePreview(originalTexture, 128);
        }
    }

    /**
     * Creates square texture preview from first 256 pixes on both dimensions of original texture
     * 
     * @param originalTexture       pointer to ITexture interface of original texture
     * @return pointer to ITexture interface of preview texture
     */
    video::ITexture* TextureTools::createTexturePreview256(video::ITexture* originalTexture) {
        if (originalTexture->getSize().Height < 256) {
            return 0;
        } else {
            return createTexturePreview(originalTexture, 256);
        }
    }

    /**
     * Creates square texture preview from first pixes on both dimensions of original texture
     * Usefull for texture editor/manager previews or some selection
     * 
     * @param originalTexture       pointer to ITexture interface of original texture
     * @param textureDimension      preview texture dimension on both dimensions
     * @return pointer to ITexture interface of preview texture
     */
    video::ITexture* TextureTools::createTexturePreview(video::ITexture* originalTexture, u32 textureDimension) {
        io::path previewTextureName = originalTexture->getName();
        previewTextureName += textureDimension;
        video::ITexture* previewTexture =
                pVideoDriver->addTexture(core::dimension2du(textureDimension, textureDimension),
                previewTextureName,
                originalTexture->getColorFormat());
        video::SColor* originalTexturePixels = (video::SColor*)originalTexture->lock(video::ETLM_READ_ONLY);
        video::SColor* previewTexturePixels = (video::SColor*)previewTexture->lock(video::ETLM_WRITE_ONLY);
        for (u32 i = 0; i < textureDimension; ++i) {
            for (u32 j = 0; j < textureDimension; ++j) {
                previewTexturePixels[i * textureDimension + j] = originalTexturePixels[i * textureDimension + j];
            }
        }
        originalTexture->unlock();
        previewTexture->unlock();

        return previewTexture;
    }

} // end of namespace irr