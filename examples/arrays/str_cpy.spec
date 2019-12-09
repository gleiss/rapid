// variation of copy_nonzero

func main()
{
	Int[] a;
	const Int[] b;
	const Int length;

	Int i = 0;
	while(i < length && b[i] != 0)
	{
		a[i] = b[i];
		i = i + 1;
	}
}

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 pos)
				(< pos (i main_end))
				(<= 0 length)
			)
			(= (a main_end pos) (b pos) )
		)
	)
)

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 length)
				(<= 0 pos)
				(< pos (i main_end))
			)
			(not (= (a main_end pos) 0))
		)
	)
)
