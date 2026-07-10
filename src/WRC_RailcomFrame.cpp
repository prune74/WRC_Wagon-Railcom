#include "WRC_RailcomFrame.h"

WRC_RailcomFrame::WRC_RailcomFrame()
{
    reset();
}

void WRC_RailcomFrame::reset()
{
    valide  = false;
    canal   = CANAL_1;
    adresse = 0;
    fxBits  = 0;
    flags   = 0;
}

String WRC_RailcomFrame::toString() const
{
    String s;

    s += "RailCom[";
    s += "canal=";
    s += (canal == CANAL_1 ? "1" : "2");
    s += ", valide=";
    s += (valide ? "true" : "false");

    if (canal == CANAL_1)
    {
        s += ", adresse=";
        s += adresse;
        s += ", flags=";
        s += flags;
    }
    else
    {
        s += ", fxBits=0x";
        s += String(fxBits, HEX);
    }

    s += "]";

    return s;
}
