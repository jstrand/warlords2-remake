
char * __cdecl16far get_error_string(int group,int index)

{
  char *pcVar1;
  
  pcVar1 = string_lookup(2,group,index);
  return pcVar1;
}


