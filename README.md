# FuncTrace

Outputs for `make && ./build-release/leveldb_trace`:

```cpp
> main
  > std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string<std::nullptr_t>(char const*)
    > std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string<std::nullptr_t>(char const*)
      > std::__1::__compressed_pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__rep, std::__1::allocator<char> >::__compressed_pair<std::__1::__default_init_tag, std::__1::__default_init_tag>(std::__1::__default_init_tag&&, std::__1::__default_init_tag&&)
        > std::__1::__compressed_pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__rep, std::__1::allocator<char> >::__compressed_pair<std::__1::__default_init_tag, std::__1::__default_init_tag>(std::__1::__default_init_tag&&, std::__1::__default_init_tag&&)
          > std::__1::__default_init_tag&& std::__1::forward<std::__1::__default_init_tag>(std::__1::remove_reference<std::__1::__default_init_tag>::type&)
          < std::__1::__default_init_tag&& std::__1::forward<std::__1::__default_init_tag>(std::__1::remove_reference<std::__1::__default_init_tag>::type&)
          > std::__1::__compressed_pair_elem<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__rep, 0, false>::__compressed_pair_elem(std::__1::__default_init_tag)
          < std::__1::__compressed_pair_elem<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__rep, 0, false>::__compressed_pair_elem(std::__1::__default_init_tag)
          > std::__1::__default_init_tag&& std::__1::forward<std::__1::__default_init_tag>(std::__1::remove_reference<std::__1::__default_init_tag>::type&)
          < std::__1::__default_init_tag&& std::__1::forward<std::__1::__default_init_tag>(std::__1::remove_reference<std::__1::__default_init_tag>::type&)
          > std::__1::__compressed_pair_elem<std::__1::allocator<char>, 1, true>::__compressed_pair_elem(std::__1::__default_init_tag)
            > std::__1::allocator<char>::allocator()
              > std::__1::__non_trivial_if<true, std::__1::allocator<char> >::__non_trivial_if()
              < std::__1::__non_trivial_if<true, std::__1::allocator<char> >::__non_trivial_if()
            < std::__1::allocator<char>::allocator()
          < std::__1::__compressed_pair_elem<std::__1::allocator<char>, 1, true>::__compressed_pair_elem(std::__1::__default_init_tag)
        < std::__1::__compressed_pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__rep, std::__1::allocator<char> >::__compressed_pair<std::__1::__default_init_tag, std::__1::__default_init_tag>(std::__1::__default_init_tag&&, std::__1::__default_init_tag&&)
      < std::__1::__compressed_pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::__rep, std::__1::allocator<char> >::__compressed_pair<std::__1::__default_init_tag, std::__1::__default_init_tag>(std::__1::__default_init_tag&&, std::__1::__default_init_tag&&)
      > std::__1::char_traits<char>::length(char const*)
      < std::__1::char_traits<char>::length(char const*)
    < std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string<std::nullptr_t>(char const*)
  < std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::basic_string<std::nullptr_t>(char const*)
  > read
      = read(5, 0x15380a600, 8192) = 16
  < read
  > read
      = read(5, 0x15380a600, 8192) = 0
  < read
  > read
      = read(5, 0x15380c600, 32768) = 106
  < read
  > read
      = read(5, 0x15380a600, 32768) = 30
  < read
  > write
      = write(5, 0x158028008, 32) = 32
  < write
  > write
      = write(5, 0x158028008, 88) = 88
  < write
  > write
      = write(6, 0x148020008, 91) = 91
  < write
  > write
      = write(6, 0x148020008, 43) = 43
  < write
  > write
      = write(7, 0x148038008, 16) = 16
  < write
  > std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::~basic_string()
  < std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::~basic_string()
  > leveldb::Status::ok() const
  < leveldb::Status::ok() const
  > leveldb::WriteOptions::WriteOptions()
    > leveldb::WriteOptions::WriteOptions()
    < leveldb::WriteOptions::WriteOptions()
  < leveldb::WriteOptions::WriteOptions()
  > leveldb::Slice::Slice(char const*)
    > leveldb::Slice::Slice(char const*)
    < leveldb::Slice::Slice(char const*)
  < leveldb::Slice::Slice(char const*)
  > leveldb::Slice::Slice(char const*)
    > leveldb::Slice::Slice(char const*)
    < leveldb::Slice::Slice(char const*)
  < leveldb::Slice::Slice(char const*)
  > write
      = write(5, 0x148008008, 30) = 30
  < write
  > leveldb::Status::~Status()
    > leveldb::Status::~Status()
    < leveldb::Status::~Status()
  < leveldb::Status::~Status()
  > leveldb::Status::~Status()
    > leveldb::Status::~Status()
    < leveldb::Status::~Status()
  < leveldb::Status::~Status()
< main
```