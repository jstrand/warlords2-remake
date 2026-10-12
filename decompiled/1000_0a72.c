
void __cdecl16far bios_set_cursor(int col,int row)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x10);
  (*pcVar1)();
  return;
}


