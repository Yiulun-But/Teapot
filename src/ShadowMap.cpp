#include "ShadowMap.h"

ShadowMap::ShadowMap(int width, int height) : m_width(width), m_height(height)
{
    Init();
};

void ShadowMap::Init()
{
    glGenFramebuffers( 1, &m_shadowFramebuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, m_shadowFramebuffer );

    glGenTextures( 1, &m_depthMap );
    glBindTexture( GL_TEXTURE_2D, m_depthMap );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glBindFramebuffer( GL_FRAMEBUFFER, m_shadowFramebuffer );
    glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthMap, 0);

    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );

    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
        fprintf(stderr, "Shadow map frame buffer initialization failed.\n");
        exit(1);
    }

}
void ShadowMap::bindFramebuffer() {
    glBindFramebuffer( GL_FRAMEBUFFER, m_shadowFramebuffer );
};
