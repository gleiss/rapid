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
				(<= 0 length)
				(<= 0 pos)
				(< pos (i main_end))
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

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 length)
				(<= 0 pos)
				(< pos (i main_end))
			)
			(not (= (b main_end pos) 0))
		)
	)
)

(conjecture
	(=>
		(and
			(<= 0 length)
			(< (i main_end) length)
		)
		(= (b main_end (i main_end)) 0)
	)
)
