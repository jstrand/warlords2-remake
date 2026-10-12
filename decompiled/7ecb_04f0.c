
char * __cdecl16far get_string(int group,int index)

{
  char *pcVar1;
  
  pcVar1 = string_lookup(0,group,index);
  return pcVar1;
}


