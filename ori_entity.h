#ifndef ORI_ENTITY_H
#define ORI_ENTITY_H

#include "cursor.h"
#include <string>

class OriEntity {

  protected:
    std::string text_color;
    std::string background_color;

    /* TODO: This isnot used */
    struct cursor cursor;

  public:
    void set_text_color (unsigned char, unsigned char, unsigned char) ;
    void set_background_color (unsigned char, unsigned char, unsigned char);
    virtual void render () {}
    virtual void do_command (struct cursor&, unsigned, char) {}
    
    /*  Pure Virtual Functions:
     *    They must be implemented by the child class
     */
    virtual void mount_cursor (struct cursor&) = 0;
    virtual struct cursor & unmount_cursor (struct cursor &) = 0;
};

#endif
