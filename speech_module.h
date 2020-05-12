#ifndef Speech_module_h



const char str1[] = "Welcome";
const char str2[] = "Main Menu";
const char str3[] = "UV Rating";
const char str4[] = "Tanning Session";
const char str5[] = "Set Timer";
const char str6[] = "Adjust Timer";
const char str7[] = "Run Timer";
const char str8[] = "Please Flip";
const char str9[] = "Tanning Done";
const char str10[] = "Apply Sunscreen";
const char str11[] = "Thank you for using smart chair";
const char star12[] = "Something Wrong";


void serial_init(unsigned short MYUBRR);
void serial_out(char ch);
void serial_out_string(char *s);
void emic_stringout(char *str);

#define Speech_module_h
#endif
