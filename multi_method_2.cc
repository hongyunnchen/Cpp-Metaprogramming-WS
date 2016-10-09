#include <iostream>
#include <type_traits>

namespace meta {
template <typename... T> struct list{};
template <typename F, typename T> struct has_type;

template <typename F>
struct has_type<F, list<>> {
  using type = typename std::false_type;
  static constexpr bool value = false;
};

template <typename F, typename... T>
struct has_type<F, list<F, T...>> {
  using type = typename std::true_type;
  static constexpr bool value = true;
};

template <typename F, typename H, typename... T>
struct has_type<F, list<H,T...>> {
  using type = typename std::false_type;
  static constexpr bool value =
    std::is_same<F, typename std::decay<H>::type>::value ? true : has_type<F, list<T...>>::value;
};
}


struct CommandBase { virtual void dummy(){};};

struct CommandA: CommandBase {};
struct CommandB: CommandBase {};
struct SomeCommandType: CommandBase {};
struct CommandC : CommandBase {};

using AvailableCommmands = meta::list<CommandA, CommandB, SomeCommandType>;

class Base
{
public:
  template <typename T>
  void on_modify_command(T cmd) {
    on_modify_command_impl(cmd, typename meta::has_type<T, AvailableCommmands>::type());
  }

  template <typename T>
  void on_modify_command_impl(T cmd, std::true_type) {
  }

  template <typename T>
  void on_modify_command_impl(T cmd, std::false_type) {
    std::cout << "Not implemented" << std::endl;
  }

  virtual void do_on_modify_command(CommandBase&) = 0;
};

class Derived: public Base
{
public:
  void do_on_modify_command(CommandBase& cmd, std::true_type) {
  }
  void impl(SomeCommandType cmd) {
    std::cout << "huh!!" << std::endl;
  }

};

class Derived2: public Base
{
  void do_on_modify_command(CommandBase& cmd, std::true_type) {
  }

  void impl(CommandA& cmd) {
  }

  void impl(CommandB& cmd) {
    std::cout << "CommandB !!" << std::endl;
  }
};

int main() {
  CommandA ca;
  Base* b = new Derived;
  b->on_modify_command(ca);
  CommandC cc;
  b->on_modify_command(cc);

  Base* b2 = new Derived2;
  CommandA aa;
  b2->on_modify_command(aa);
  CommandB bb;
  b2->on_modify_command(bb);

  return 0;
}
