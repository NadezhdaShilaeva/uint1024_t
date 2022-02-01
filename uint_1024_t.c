#include <stdio.h>
#include <ctype.h>

typedef struct
{
	unsigned int part_number[32];
} uint1024_t;

uint1024_t from_uint(unsigned int x)
{
	uint1024_t x_new;
	int i;
	for (i = 0; i < 32; ++i)
	{
		x_new.part_number[i] = 0;
	}
	for (i = 0; x > 0; ++i)
	{
		if (x % 2 == 1)
		{
			x_new.part_number[0] = x_new.part_number[0] | (1 << (31 - i));
		}
		x /= 2;
	}
	return x_new;
}

uint1024_t add_op(uint1024_t x, uint1024_t y)
{
	uint1024_t ans;
	char overflow = 0;
	for (int i = 0; i < 32; ++i)
	{
		ans.part_number[i] = 0;
	}
	for (int i = 0; i < 32; ++i)
	{	
		for (int j = 0; j < 32; ++j)
		{
			ans.part_number[i] = ans.part_number[i] ^ (x.part_number[i] & (1 << (31 - j)));
			ans.part_number[i] = ans.part_number[i] ^ (y.part_number[i] & (1 << (31 - j)));
			if (overflow == 1)
			{
				ans.part_number[i] = ans.part_number[i] ^ (1 << (31 - j));
			}
			if (((x.part_number[i] & (1 << (31 - j))) && (y.part_number[i] & (1 << (31 - j))))
				|| (overflow && ((x.part_number[i] & (1 << (31 - j))) || (y.part_number[i] & (1 << (31 - j))))))
			{
				overflow = 1;
			}
			else
			{
				overflow = 0;
			}
		}
	}
	return ans;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y)
{
	uint1024_t ans;
	for (int i = 0; i < 32; ++i)
	{
		ans.part_number[i] = 0;
	}
	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j<32; ++j)
		{
			if ((x.part_number[i] & (1 << (31 - j))) < (y.part_number[i] & (1 << (31 - j))))
			{
				for (int l = i * 32 + j + 1; l < 32 * 32; ++l)
				{
					if (x.part_number[l / 32] & (1 << (31 - l % 32)))
					{
						x.part_number[l / 32] = x.part_number[l / 32] & (~(1 << (31 - l % 32)));
						ans.part_number[i] = ans.part_number[i] | (1 << (31 - j));
						break;
					}
					x.part_number[l / 32] = x.part_number[l / 32] | (1 << (31 - l % 32));
				}
			}
			else
			{
				ans.part_number[i] = ans.part_number[i] | ((x.part_number[i] & (1 << (31 - j))) ^ (y.part_number[i] & (1 << (31 - j))));
			}
		}
	}
	return ans;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y)
{
	uint1024_t ans;
	for (int i = 0; i < 32; ++i)
	{
		ans.part_number[i] = 0;
	}
	int j = 0;
	for (int i = 0; i < 32 * 32; ++i)
	{
		if (y.part_number[i / 32] & (1 << (31 - i % 32)))
		{
			while(j<i)
			{
				for (int l = 31; l > 0; --l)
				{
					x.part_number[l] = x.part_number[l] >> 1;
					x.part_number[l] = x.part_number[l] | (x.part_number[l - 1] << 31);
				}
				x.part_number[0] = x.part_number[0] >> 1;
				++j;
			}
			ans = add_op(ans, x);
		}
	}
	return ans;
}

void printf_value(uint1024_t x)
{
	int i;
	for (i = 1024 - 1; i > 0; --i)
	{
		if (x.part_number[i / 32] & (1 << (31 - i % 32)))
		{
			break;
		}
	}
	for (i = i + 3 - i % 4; i > 0; --i)
	{
		int number_hex;
		number_hex = 8 * ((x.part_number[i / 32] >> (31 - i % 32)) & 1);
		--i;
		number_hex += 4 * ((x.part_number[i / 32] >> (31 - i % 32)) & 1);
		--i;
		number_hex += 2 * ((x.part_number[i / 32] >> (31 - i % 32)) & 1);
		--i;
		number_hex += ((x.part_number[i / 32] >> (31 - i % 32)) & 1);
		if (number_hex < 10)
		{
			printf("%d", number_hex);
		}
		else
		{
			printf("%c", 'A' + number_hex % 10);
		}
	}
}

void scanf_value(uint1024_t* x)
{
	char symbol;
	scanf("%c", &symbol);
	for (int i = 0; isdigit(symbol); ++i)
	{
		unsigned int value = symbol - '0';
		*x = mult_op(*x, from_uint(10));
		*x = add_op(*x, from_uint(value));
		scanf("%c", &symbol);
	}
	ungetc(symbol, stdin);
}

int main()
{
	uint1024_t x, y, ans;
	for (int i = 0; i < 32; ++i)
	{
		x.part_number[i] = 0;
		y.part_number[i] = 0;
		ans.part_number[i] = 0;
	}
	char operator_;
	scanf_value(&x);
	scanf(" %c ", &operator_);
	scanf_value(&y);
	if (operator_ == '+')
	{
		ans = add_op(x, y);
	}
	else if (operator_ == '-')
	{
		ans = subtr_op(x, y);
	}
	else if (operator_ == '*')
	{
		ans = mult_op(x, y);
	}
	else
	{
		printf("Error of operator");
		return -1;
	}
	printf_value(ans);
	return 0;
}