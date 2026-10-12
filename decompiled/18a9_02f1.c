
bool __cdecl16far FUN_18a9_02f1(void)

{
  int iVar1;
  bool bVar2;
  undefined4 local_6;
  
  local_6 = (undefined2 *)
            CONCAT22(((int *)DAT_451b_0ce0)[1],(undefined2 *)(*DAT_451b_0ce0 + DAT_451b_0cdc * 0x21)
                    );
  iVar1 = FUN_18a9_0756();
  bVar2 = iVar1 != DAT_451b_0cdc;
  if (bVar2) {
    FUN_18a9_066e(*local_6,1);
  }
  else {
    FUN_18a9_066e(*local_6,0);
  }
  return !bVar2;
}


