#ifndef ORI_LINE_H
#define ORI_LINE_H

#include <list>
#include <string>

struct file_piece {
  bool from_read_only;  // which buffer piece refers to
  size_t pos;           // pos into buffer to start reading from
  size_t length;        // length to read from buffer
};

class Line {
  private:
    /*TODO: delete text */
    std::string text;

    std::list<struct file_piece>* pieces;
    std::string mark = "[ ]";

    std::string frame_buffer;
    bool frame_cached = false;
  
  public:
    Line (bool, size_t, size_t);
    Line (std::list<struct file_piece> *);
    std::string &pieces_to_string (const std::string &, const std::string &);

    Line (char*);
    Line (void);
    Line (std::string);
    Line (std::string&, std::string&);
    const char *get_str ();
    const std::string get_str_obj (); 
    unsigned length ();
    void insert_char (unsigned, size_t);
    void append (std::list<Line>::iterator &);
    void delete_char (unsigned);
    std::list<struct file_piece>* clip (unsigned);
    std::string substr (unsigned, unsigned);
    const char* get_mark ();
    void set_mark (char);
    void draw (unsigned);
    void draw_color (unsigned, const std::string &, const std::string &);
};

void lexer_reset ();

#endif

