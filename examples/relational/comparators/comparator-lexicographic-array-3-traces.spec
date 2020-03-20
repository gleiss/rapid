(set-traces 3)

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
	(<= 0 (length t1))
)
(axiom
	(<= 0 (length t2))
)
(axiom
	(<= 0 (length t3))
)


(conjecture
	(=>
		(and
			(forall ((pos Int))
				(= (a pos t1) (a pos t3))
			)
			(forall ((pos Int))
				(= (b pos t1) (a pos t2))
			)
			(forall ((pos Int))
				(= (b pos t2) (b pos t3))
			)
			(= (length t1) (length t2))
			(= (length t2) (length t3))
			(= (length t1) (length t3))

			(= (return main_end t1) 10)
			(= (return main_end t2) 10)
		)
		(= (return main_end t3) 10)
	)
)

(conjecture
	(=>
		(and
			(forall ((pos Int))
				(= (a pos t1) (a pos t3))
			)
			(forall ((pos Int))
				(= (b pos t1) (a pos t2))
			)
			(forall ((pos Int))
				(= (b pos t2) (b pos t3))
			)
			(= (length t1) (length t2))
			(= (length t2) (length t3))
			(= (length t1) (length t3))

			(= (return main_end t1) 20)
			(= (return main_end t2) 10)
		)
		(= (return main_end t3) 20)
	)
)

(conjecture
	(=>
		(and
			(forall ((pos Int))
				(= (a pos t1) (a pos t3))
			)
			(forall ((pos Int))
				(= (b pos t1) (a pos t2))
			)
			(forall ((pos Int))
				(= (b pos t2) (b pos t3))
			)
			(= (length t1) (length t2))
			(= (length t2) (length t3))
			(= (length t1) (length t3))

			(= (return main_end t1) 20)
			(= (return main_end t2) 20)
		)
		(= (return main_end t3) 20)
	)
)