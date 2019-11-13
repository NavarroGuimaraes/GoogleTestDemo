#include "pch.h"
#include <gtest/gtest.h>
#include "test.h"

struct BankAccount
{
	int balance = 0;

	BankAccount(){
	}

	explicit BankAccount(const int balance) 
		: balance{ balance } 
	{

	}

	void deposit(int amount) {
		balance += amount;
	}

	bool withdraw(int amount) 
	{
		if (amount <= balance) {
			balance -= amount;
			return true;
		}
		else {
			return false;
		}
	};
};

struct BankAccountTest : testing::Test 
{
	//Cria o campo account
	BankAccount* account;

	//Constrói uma instancia da classe BankAccount
	BankAccountTest()
	{
		account = new BankAccount;
	}

	//Destrói a instância 
	~BankAccountTest()
	{
		delete account;
	};
};

//Usamos a função TEST_F quando usamos uma fixture, que nos permitirá acessar objetos na rotina de teste.
//Nesse caso iremos utilizar a fixture de conta bancária para não ter que repetir sempre que quisermos 
//criar um novo teste. Assim, o grupo do teste se torna a fixture que será utilizada no teste
//O primeiro paRâmetro é a fixture e o segundo é o nome do teste (pode ser qualquer um)
//Aqui testamos se a conta bancária está se inicializando corretamente
TEST_F(BankAccountTest, BankAccountStartsEmpty)
{
	EXPECT_EQ(0, account->balance);
}

//Mesmo caso do anterior. Aqui iremos testar se o deposito funcionará.
TEST_F(BankAccountTest, canDepositMoney)
{
	account->deposit(100);
	EXPECT_EQ(100, account->balance);
}

//Para testar os métodos de saque, iremos criar set de dados para teste.
//Escrevemos esse teste parametrizado pois essencialmente teremos que testar alguns cenários
//onde teremos ou não teremos dinheiro suficiente para sacar.
//O Item de teste de dados que iremos utilziar será account_state
struct account_state
{
	//O balanço inicial da conta
	int initial_balance;
	//O tanto que queremos sacar
	int withdraw_amount;
	//O balanço final
	int final_balance;
	//Se teremos ou não sucesso em nossa operação de saque
	bool success;

	friend std::ostream& operator<<(std::ostream& os, const account_state& obj)
	{
		return os
			<< "initial_balance: " << obj.initial_balance
			<< " withdraw_amount: " << obj.withdraw_amount
			<< " final_balance: " << obj.final_balance
			<< " success: " << obj.success;
	}
};

//Visto que teremos que criar várias instâncias do set de dados acima, para evitar sempre criá-los
//Iremos criar mais uma fixture que será um pouco diferente, pois queremos continuar usando a fixture
//da conta bancária
//Então, criamos uma fixture herdando da conta bancária e da interface que criamos acima. Assim, passamos
//o argumento template account_state em WithParamInterface.
struct WithdrawAccountTest : BankAccountTest, testing::WithParamInterface<account_state>
{
	//Cria o construtor para criar o valor inicial da conta
	WithdrawAccountTest() {
		account->balance = GetParam().initial_balance;
	}
};

//A função test_p é chamada quando a classe fixture herda de ambos testing::Test 
//e testing::WithParamInterface<T>. Como precisamos criar uma interface para os valores da conta
//Foi necessário utilizar a função TEST_P. O sufixo P remete a "Parametrizado".
TEST_P(WithdrawAccountTest, FinalBalance)
{
	auto as = GetParam();
	auto success = account->withdraw(as.withdraw_amount);
	EXPECT_EQ(as.final_balance, account->balance);
	EXPECT_EQ(as.success, success);
}

//Para inciiar os testes do tipo parametrizado, é necessário instanciar a interface
//passando os dados desejados.
//Como parâmetros passando um prefixo que pode ser qualquer nome da instansciação
//Depois, passamos a fixture que possui a interface (set de dados) e a fixture da conta bancária
//Depois, passamos uma função interna do google test testing::values() com os valores desejados como
//argumentos da função.
//Na instância que passamos, colocamos os valores:
//Valor inicial, valor do saque, valor final e se deve dar sucesso
INSTANTIATE_TEST_CASE_P(Default, WithdrawAccountTest, testing::Values(
	account_state{100, 50, 50, true},
	account_state{100, 200, 100, false},
	account_state{150, 50, 100, true}
));

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}