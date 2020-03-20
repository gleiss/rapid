func main()
{
	const Int[] a;
	const Int[] b;
	const Int length;

	// return value: 20 encodes a<b, 10 encodes a=b, 30 encodes b<a
	Int return;

	Int i = 0;
	while(i < length && a[i] == b[i])
	{
		i = i + 1;
	}

	if (i < length)
	{
		if (a[i] < b[i])
		{
			return = 20;
		}
		else
		{
			return = 30;
		}
	}
	else
	{
		return = 10;
	}
}

(axiom
	(<= 0 length)
)

(conjecture
	(=>
		(forall ((pos Int))
			(= (a pos) (b pos))
		)
		(= (return main_end) 10)
	)
)

(conjecture
	(=>
		(= (return main_end) 10)
		(forall ((pos Int))
			(=>
				(and
					(<= 0 pos)
					(< pos length)
				)
				(= (a pos) (b pos))
			)
		)
	)
)

(conjecture
	(=>
		(exists ((pos Int))
			(and
				(<= 0 pos)
				(< pos length)
				(forall ((pos2 Int))
					(=>
						(and
							(<= 0 pos2)
							(< pos2 pos)
						)
						(= (a pos2) (b pos2))
					)
				)
				(< (a pos) (b pos))
			)
		)
		(= (return main_end) 20)
	)
)

(conjecture
	(=>
		(= (return main_end) 20)
		(exists ((pos Int))
			(and
				(forall ((pos2 Int))
					(=>
						(and
							(<= 0 pos2)
							(< pos2 pos)
						)
						(= (a pos2) (b pos2))
					)
				)
				(< (a pos) (b pos))
			)
		)
	)
)