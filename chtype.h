#ifndef CHTYPE_H
#define CHTYPE_H

bool is_point(char c);
bool is_quot(char c);
bool is_escape(char c);
bool is_escapable(char c);
bool is_spec_beg(char c);
bool is_spec_end(char c);
bool is_num_quant(char c);
bool is_id_quant(char c);
bool is_comment_beg(char c);
bool is_comment_end(char c);

#endif
