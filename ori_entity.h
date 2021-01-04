#ifndef ORI_ENTITY_H
#define ORI_ENTITY_H

#include "ori_codes.h"
#include "cursor.h"
#include <string>

class OriEntity {

  protected:
    std::string text_color;
    std::string background_color;

    /* TODO: This isnot used */
    struct cursor cursor;
    virtual unsigned command_up () {return ORI_NO_OP;}
    virtual unsigned command_down () {return ORI_NO_OP;}
    virtual unsigned command_left () {return ORI_NO_OP;}
    virtual unsigned command_right () {return ORI_NO_OP;}
    virtual unsigned command_pgup () {return ORI_NO_OP;}
    virtual unsigned command_pgdown () {return ORI_NO_OP;}
    virtual unsigned command_enter () {return ORI_NO_OP;}
    virtual unsigned command_backspace () {return ORI_NO_OP;}

  public:
    virtual ~OriEntity () = default;
    void set_text_color (unsigned char, unsigned char, unsigned char) ;
    void set_background_color (unsigned char, unsigned char, unsigned char);
    const struct cursor & get_cursor ();

    /* TODO: make this in oriEntity.cpp. chidren will inherent */
    virtual unsigned do_command (unsigned, char) {}
    
    /*  Pure Virtual Functions:
     *    They must be implemented by the child class
     */
    virtual void mount_cursor () = 0;
    virtual struct cursor & unmount_cursor () = 0;
    virtual void render () = 0;
};

#endif
