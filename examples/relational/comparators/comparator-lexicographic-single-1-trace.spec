func main()
{
	const Int a;
	const Int b;

	// return value: 20 encodes a<b, 10 encodes a=b, 30 encodes b<a
	Int return;

	if (a != b)
	{
		if (a < b)
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

(conjecture
	(=>
		(= a b)
		(= (return main_end) 10)
	)
)

(conjecture
	(=>
		(= (return main_end) 10)
		(= a b)
	)
)

(conjecture
	(=>
		(< a b)
		(= (return main_end) 20)
	)
)

(conjecture
	(=>
		(= (return main_end) 20)
		(< a b)
	)
)