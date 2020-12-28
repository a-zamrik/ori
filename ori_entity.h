#ifndef ORI_ENTITY_H
#define ORI_ENTITY_H


class OriEntity {
  public:
    virtual void render () {}
    virtual void do_command (struct cursor&, unsigned, char) {}
};

#endif
