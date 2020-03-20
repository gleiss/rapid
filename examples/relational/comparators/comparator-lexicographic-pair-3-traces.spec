(set-traces 3)

func main()
{
	const Int a1;
	const Int a2;
	const Int b1;
	const Int b2;

	// return value: 20 encodes a<b, 10 encodes a=b, 30 encodes b<a
	Int return;

	if (a1 != b1)
	{
		if (a1 < b1)
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
		if (a2 != b2)
		{
			if (a2 < b2)
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
}

(conjecture
	(=>
		(and
			(= (a1 t1) (a1 t3))
			(= (a2 t1) (a2 t3))
			(= (b1 t1) (a1 t2))
			(= (b2 t1) (a2 t2))
			(= (b1 t2) (b1 t3))
			(= (b2 t2) (b2 t3))

			(= (return main_end t1) 10)
			(= (return main_end t2) 10)
		)
		(= (return main_end t3) 10)
	)
)

(conjecture
	(=>
		(and
			(= (a1 t1) (a1 t3))
			(= (a2 t1) (a2 t3))
			(= (b1 t1) (a1 t2))
			(= (b2 t1) (a2 t2))
			(= (b1 t2) (b1 t3))
			(= (b2 t2) (b2 t3))

			(= (return main_end t1) 10)
			(= (return main_end t2) 20)
		)
		(= (return main_end t3) 20)
	)
)

(conjecture
	(=>
		(and
			(= (a1 t1) (a1 t3))
			(= (a2 t1) (a2 t3))
			(= (b1 t1) (a1 t2))
			(= (b2 t1) (a2 t2))
			(= (b1 t2) (b1 t3))
			(= (b2 t2) (b2 t3))

			(= (return main_end t1) 20)
			(= (return main_end t2) 10)
		)
		(= (return main_end t3) 20)
	)
)


(conjecture
	(=>
		(and
			(= (a1 t1) (a1 t3))
			(= (a2 t1) (a2 t3))
			(= (b1 t1) (a1 t2))
			(= (b2 t1) (a2 t2))
			(= (b1 t2) (b1 t3))
			(= (b2 t2) (b2 t3))

			(= (return main_end t1) 20)
			(= (return main_end t2) 20)
		)
		(= (return main_end t3) 20)
	)
)