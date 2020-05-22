#ifndef TRACE_H
#define TRACE_H

//#define DEBUG

#ifdef DEBUG
#define TRACE(cadena) Serial.println("DEBUG: "  cadena);Serial.flush()
#else
//#error ARRRRRRG
#define TRACE(cadena) {}
#endif

#endif
