func main()
{
	const Int[] a;
    const Int[] b;
    const Int[] c;
    const Int length;

	Int i = 0;
    Int j = 0;
    Int k = 0;

	Int return1;
	Int return2;
	Int return3;

	while(a[i] == b[i] && i < length)
	{
		i = i + 1;
	}

    if (i < length)
	{
		if (a[i] < b[i])
		{
			return1 = 1;
		}
		else
		{
			return1 = 2;
		}
	}
	else
	{
		return1 = 0;
	}

    while(b[j] == c[j] && j < length)
	{
		j = j + 1;
	}

	if (j < length)
   {
	   if (b[j] < c[j])
	   {
		   return2 = 1;
	   }
	   else
	   {
		   return2 = 2;
	   }
   }
   else
   {
	   return2 = 0;
   }

    while(a[k] == c[k] && k < length)
	{
		k = k + 1;
	}

	if (k < length)
	{
	   if (a[k] < c[k])
	   {
		   return3 = 1;
	   }
	   else
	   {
		   return3 = 2;
	   }
	}
    else
    {
	    return3 = 0;
    }

}

(conjecture
	(=>
		(and
			(= (return1 main_end) 1)
			(= (return2 main_end) 1)
		)
		(= (return3 main_end) 1)
	)
)
