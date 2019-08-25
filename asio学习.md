# asio学习

### 基于行的操作

由于许多协议是基于行的，为了更容易地允许实现基于行的协议，以及使用分隔符的其他协议，asio使用`read_until()`和`async_read_until()`

实例如下：

~~~c++
class http_connection
{
  ...

  void start()
  {
    asio::async_read_until(socket_, data_, "\r\n",
        boost::bind(&http_connection::handle_request_line, this, _1));
  }

  void handle_request_line(asio::error_code ec)
  {
    if (!ec)
    {
      std::string method, uri, version;
      char sp1, sp2, cr, lf;
      std::istream is(&data_);
      is.unsetf(std::ios_base::skipws);
      is >> method >> sp1 >> uri >> sp2 >> version >> cr >> lf;
      ...
    }
  }

  ...

  asio::ip::tcp::socket socket_;
  asio::streambuf data_;
};
~~~

这个分隔符可以是单个char，string或者regex。另外read_until和async_read_until也接受用户定义的函数对象作为匹配条件

示例：

~~~c++
typedef asio::buffers_iterator<
    asio::streambuf::const_buffers_type> iterator;

std::pair<iterator, bool>
match_whitespace(iterator begin, iterator end)
{
  iterator i = begin;
  while (i != end)
    if (std::isspace(*i++))
      return std::make_pair(i, true);
  return std::make_pair(i, false);
}
...
asio::streambuf b;
asio::read_until(s, b, match_whitespace);
~~~

例如匹配一个字符的示例程序如下：

~~~c++
class match_char
{
public:
  explicit match_char(char c) : c_(c) {}

  template <typename Iterator>
  std::pair<Iterator, bool> operator()(
      Iterator begin, Iterator end) const
  {
    Iterator i = begin;
    while (i != end)
      if (c_ == *i++)
        return std::make_pair(i, true);
    return std::make_pair(i, false);
  }

private:
  char c_;
};

namespace asio {
  template <> struct is_match_condition<match_char>
    : public boost::true_type {};
} // namespace asio
...
asio::streambuf b;
asio::read_until(s, b, match_char('a'));
~~~

该类型继承true_type，

