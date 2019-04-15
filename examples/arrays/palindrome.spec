func main()
{
	Int[] a;
  const Int alength;

	Int i = 0;
	while(i < (alength / 2))
	{
    a[i] = a[(alength - i) - 1];
    i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos (/ alength 2))
          (<= 0 alength)
        )
        (= (a main_end pos) (a main_end (- (- alength pos ) 1) ))
      )
   )
)
