func main()
{
  const Int[] a;
  const Int alength;

  Int[] b;
  Int[] c;

	Int i = 0;
  Int j = 0;
  Int k = 0;
	while(i < alength)
	{
    if(a[i] >= 0)
    {
      b[j] = a[i];
      j = j + 1;
    }
    else
    {
      c[k] = a[i];
      k = k + 1;
    }
    i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos (j main_end))
          (<= 0 alength)
        )
        (<= 0 (b main_end pos))
      )
   )
)

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos (k main_end))
          (<= 0 alength)
        )
        (< (c main_end pos) 0)
      )
   )
)
