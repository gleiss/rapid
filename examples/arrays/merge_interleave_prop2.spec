func main()
{
	const Int[] a;
  const Int[] b;
  Int[] c;
	const Int alength;

	Int i = 0;
	while(i < alength)
	{
    c[i * 2] = a[i];
    c[i * 2 + 1] = b[i];
		i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos alength)
          (<= 0 alength)
        )
        (= (c main_end (+ (* pos 2) 1)) (b pos))
      )
   )
)
