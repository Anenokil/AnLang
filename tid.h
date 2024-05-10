#ifndef TID_H
#define TID_H

#include <string>
#include <vector>

enum id_types {
    ID_TYPE_UNKNOWN,
    ID_TYPE_INT,
    ID_TYPE_FLOAT,
    ID_TYPE_BOOL,
    ID_TYPE_STR,
};

class TID
{
private:
    class TID_Line
    {
    private:
        id_types type_{};
        std::string name_{};
        std::string init_{};

    public:
        TID_Line(std::string const & type_raw, std::string const & name_raw, std::string const & init_raw);
        void set_type(std::string const & type_raw);
        void set_name(std::string const & name_raw);
        void set_init(std::string const & init_raw);
        id_types get_type() const;
        std::string get_name() const;
        std::string get_init() const;
        bool operator==(TID_Line const & other) const;
        bool operator!=(TID_Line const & other) const;
        void print() const;
    };

    std::vector<TID_Line> lines_{};

public:
    bool add(std::string const & type = "", std::string const & name = "", std::string const & init = "");
    bool set_init(std::string const & name, std::string const & init);
    void print() const;
};

#endif
