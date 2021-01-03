#ifndef ORI_LINE_H
#define ORI_LINE_H

#include <list>
#include <string>

class Line {
  private:
    std::string text;
    std::string mark = "[ ]";

    std::string frame_buffer;
    bool frame_cached = false;
  
  public:
    Line (char*);
    Line (void);
    Line (std::string);
    Line (std::string&, std::string&);
    const char *get_str ();
    const std::string get_str_obj (); 
    unsigned length ();
    void insert_char (char, unsigned);
    void append (std::list<Line>::iterator &);
    void delete_char (unsigned);
    bool clip (unsigned);
    std::string substr (unsigned, unsigned);
    const char* get_mark ();
    void set_mark (char);
    void draw (unsigned);
    void draw_color (unsigned);
};

#endif

