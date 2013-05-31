#include "strings.h"

wavrStrings::wavrStrings(void) {
}

wavrStrings::~wavrStrings(void){
}

const QString wavrStrings::appName(void) {
    if(m_appName.isEmpty())
        m_appName = tr("Wavr");
    return m_appName;
}
