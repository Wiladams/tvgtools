#include "tvgscan.h"

using namespace tvg;

using Token_Source = std::function< tv_command_t () >;
using Token_Filter = std::function< tv_command_t (tv_command_t &cmd)>;
using Token_Sink = std::function< void (tv_command_t &cmd)>;

class TokenSource {
    tvg_scanner &fScanner;

public:
    TokenSource(tvg_scanner &scanner) : fScanner(scanner) {}

    tv_command_t operator()() {
        tvg_command_t cmd;
        if (fScanner.next(cmd))
        return src();
    }
}