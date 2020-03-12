func main()
{
	Int[] a;
	Int[] b;

	const Int length;

	Int i = 0;
	while(i < length)
	{
		Int current = a[i];
		a[i] = b[i];
		b[i] = current;
		i = i + 1;
	}
}

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 length)
				(<= 0 pos)
				(< pos length)
			)
			(= (a main_end pos) (b l8 pos))
		)
	)
)

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 length)
				(<= 0 pos)
				(< pos length)
			)
			(= (b main_end pos) (a l8 pos))
		)
	)
)