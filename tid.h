#ifndef TID_H
#define TID_H

#include <ostream>
#include <string>
#include <vector>

enum IDType {
    ID_TYPE_UNKNOWN,
    ID_TYPE_INT,
    ID_TYPE_FLOAT,
    ID_TYPE_BOOL,
    ID_TYPE_STR,
};

class TID
{
private:
    class TIDLine
    {
    private:
        IDType type_{};
        std::string name_{};
        std::string init_{};

    public:
        TIDLine(std::string const & type_raw, std::string const & name_raw, std::string const & init_raw);
        void set_type(std::string const & type_raw);
        void set_name(std::string const & name_raw);
        void set_init(std::string const & init_raw);
        IDType get_type() const;
        std::string get_typename() const;
        std::string get_name() const;
        std::string get_init() const;
        bool operator==(TIDLine const & other) const;
        bool operator!=(TIDLine const & other) const;
        std::ostream & print(std::ostream & os) const;
    };

    std::vector<TIDLine> lines_{};

public:
    int find(std::string const & name) const;
    bool add(std::string const & type = "", std::string const & name = "", std::string const & init = "");
    bool set_init(std::string const & name, std::string const & init);
    friend std::ostream & operator<<(std::ostream & os, TID const & obj);
};

#endif
