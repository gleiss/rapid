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
			(= a1 b1)
			(= a2 b2)
		)
		(= (return main_end) 10)
	)
)

(conjecture
	(=>
		(= (return main_end) 10)
		(and
			(= a1 b1)
			(= a2 b2)
		)
	)
)

(conjecture
	(=>
		(or
			(< a1 b1)
			(and
				(= a1 b1)
				(< a2 b2)
			)
		)
		(= (return main_end) 20)
	)
)

(conjecture
	(=>
		(= (return main_end) 20)
		(or
			(< a1 b1)
			(and
				(= a1 b1)
				(< a2 b2)
			)
		)
	)
)