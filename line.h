#ifndef ORI_LINE_H
#define ORI_LINE_H

#include <list>
#include <string>
#include <stack>

struct file_piece {
  bool from_read_only;  // which buffer piece refers to
  size_t pos;           // pos into buffer to start reading from
  size_t length;        // length to read from buffer

  file_piece () {
    from_read_only = false;
    pos = 0;
    length = 0;
  }
};

class Line {
  private:
    /*TODO: delete text */
    std::string text;

    std::list<struct file_piece*>* pieces;
    std::string mark = "[ ]";

    std::string frame_buffer;
    bool frame_cached = false;

    /* if mounted; a single piece is being modified. Dont create a new
     * piece, just modify the current one */
    // Line sets this true, set to false by outside
    bool piece_mounted = false;
  
  public:
    Line (bool, size_t, size_t);
    Line (std::list<struct file_piece*> *, std::stack<struct redo>*);
    std::string &pieces_to_string (const std::string &, const std::string &);
    void restore_line (struct file_piece*, Line*);

    ~Line ();

    Line (char*);
    Line (void);
    Line (std::string);
    Line (std::string&, std::string&);
    const char *get_str (const std::string &r_buf,
                         const std::string &w_buf);
    const std::string get_str_obj (const std::string &r_buf,
                                   const std::string &w_buf); 
    unsigned length ();
    void insert_char (unsigned, size_t, std::stack<struct redo>*);
    void append (std::list<Line*>::iterator &);
    void delete_char (unsigned, std::stack<struct redo>*);
    std::list<struct file_piece*>* clip (unsigned, std::stack<struct redo>*);
    std::string substr (unsigned, unsigned);
    const char* get_mark ();
    void set_mark (char);
    void draw (unsigned, const std::string &, const std::string &);
    void draw_color (unsigned, const std::string &, const std::string &);

    void unmount ();
};

struct redo {
  struct file_piece old_piece;  // old data
  struct file_piece* piece_location;  // the piece that needs to be set to old_piece
  struct file_piece* aux_location; // if aux_location is not NULL, Zero it
  struct file_piece* second_aux_location; // if not NULL, append to aux_location
  struct Line* old_line; // line that was split by ENTER
  struct Line* new_line; // line that utilized the split
};

void lexer_reset ();

#endif

