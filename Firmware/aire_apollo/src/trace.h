#ifndef TRACE_H
#define TRACE_H
#ifdef DEBUG
#define TRACE(cadena) Serial.println("DEBUG: "  cadena);Serial.flush()
#else
#define TRACE(cadena) {}
#endif

#endif
