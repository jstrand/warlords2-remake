
void __cdecl16far FUN_7ecb_01dc(void)

{
  int iVar1;
  
  while( true ) {
    iVar1 = kbhit();
    if (iVar1 == 0) break;
    getch();
  }
  return;
}


