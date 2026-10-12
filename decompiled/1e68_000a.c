
void __cdecl16far FUN_1e68_000a(void)

{
  byte bVar1;
  
  do {
    bVar1 = in(0x3da);
  } while ((bVar1 & 8) == 0);
  do {
    bVar1 = in(0x3da);
  } while ((bVar1 & 8) != 0);
  return;
}


