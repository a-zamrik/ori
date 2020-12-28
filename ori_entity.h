#ifndef ORI_ENTITY_H
#define ORI_ENTITY_H


class OriEntity {
  public:
    virtual void render () {}
    virtual void do_command (struct cursor&, unsigned, char) {}
    
    /*  Pure Virtual Functions:
     *    They must be implemented by the child class
     */
    virtual void mount_cursor (struct cursor&) = 0;
    virtual struct cursor & unmount_cursor (struct cursor &) = 0;
};

#endif
