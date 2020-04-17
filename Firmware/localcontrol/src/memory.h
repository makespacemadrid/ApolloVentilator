struct MYDATA{                                          // Estructura STRUCT con las variables que almacenaran los datos que se guardaran en la memoria EEPROM
    int initialized;
    int time_show;
    int time_unit;
    int time_x;
    int time_y;
    int temp_show;
    int temp_unit;
    int temp_x;
    int temp_y;
};
union MEMORY{                                           // Estructura UNION para facilitar la lectura y escritura en la EEPROM de la estructura STRUCT
    MYDATA d;
    byte b[sizeof(MYDATA)];
}
memory;
        /**
         *  ESCRIBE LA MEMORIA EEPROM CON AL CONFIGURACION DE USUARIO
         */
        void writeConfiguration()
        {
            for( int i=0 ; i<sizeof(memory.d) ; i++  )
                EEPROM.write( i, memory.b[i] );
        }

        /**
         *  LEE (Y CONFIGURA LA PRIMERA VEZ) LA MEMORIA EEPROM CON LA CONFIGURACION DE USUARIO
         */
        void readConfiguration()
        {
            for( int i=0 ; i < sizeof(memory.d) ; i++  )
                memory.b[i] = EEPROM.read(i);

            if( memory.d.initialized != 'Y' )
            {
                memory.d.initialized = 'Y';
                memory.d.time_show   = 1;
                memory.d.time_unit   = 1;
                memory.d.time_x      = 0;
                memory.d.time_y      = 0;
                memory.d.temp_show   = 1;
                memory.d.temp_unit   = 0;
                memory.d.temp_x      = 0;
                memory.d.temp_y      = 1;
                writeConfiguration();
            }
        }



        // enum menuTime{ Milliseconds, Seconds, Minutes, Hours };  // Enumerador de las opciones disponibles del submenu 1 (tienen que seguir el mismo orden que los textos)
        // String menuTimeTxt[4] = { "Milliseconds", "Seconds", "Minutes", "Hours" };

        // enum menuTemp{ GradeC, GradeF };                         // Enumerador de las opciones disponibles del submenu 2 (tienen que seguir el mismo orden que los textos)
        // String menuTempTxt[2] = { "GradeC", "GradeF"};
