func main()
{
  Int[] bb;
  Int[] cc;
  const Int[] aa;
  const Int alength;

	Int a = 0;
  Int b = 0;
	while(a < alength)
	{
    if(aa[a] == cc[a])
    {
      bb[b] = a;
      b = b + 1;
    }
    else
    {
      skip;
    }
    a = a + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos (b main_end))
          (<= 0 alength)
        )
        (<= 0 (bb main_end pos))
      )
   )
)
