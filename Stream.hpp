#pragma once

#include "LazySequence.hpp"
#include "Sequence.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

template <typename T> class ReadOnlyStream {
    public:
        virtual ~ReadOnlyStream() = default;

        virtual void open() = 0;
        virtual void close() = 0;
        virtual bool is_end_of_stream() const = 0;
        virtual T read() = 0;
        virtual int get_position() const = 0;
        virtual bool is_can_go_to_index() const = 0;
        virtual int go_to_index(int index) = 0;
        virtual int go_forward() = 0;
        virtual int go_back() = 0;
        virtual bool is_can_go_back() const = 0;
};

template <typename T> class WriteOnlyStream {
    public:
        virtual ~WriteOnlyStream() = default;

        virtual void open() = 0;
        virtual void close() = 0;
        virtual int write(const T &item) = 0;
        virtual int get_position() const = 0;
        virtual bool is_can_go_to_index() const = 0;
        virtual int go_to_index(int index) = 0;
        virtual int go_forward() = 0;
        virtual int go_back() = 0;
        virtual bool is_can_go_back() const = 0;
};

template <typename T> class SequenceReadStream : public ReadOnlyStream<T> {
    private:
        const Sequence<T> *source;
        int position;
        bool opened;

        void ensure_opened() const;

    public:
        SequenceReadStream(const Sequence<T> &source);

        void open() override;
        void close() override;
        bool is_end_of_stream() const override;
        T read() override;
        int get_position() const override;
        bool is_can_go_to_index() const override;
        int go_to_index(int index) override;
        int go_forward() override;
        int go_back() override;
        bool is_can_go_back() const override;
};

template <typename T> class LazySequenceReadStream : public ReadOnlyStream<T> {
    private:
        LazySequence<T> *source;
        int position;
        bool opened;

        void ensure_opened() const;
    
    public:
        LazySequenceReadStream(LazySequence<T> &source);

        void open() override;
        void close() override;
        bool is_end_of_stream() const override;
        T read() override;
        int get_position() const override;
        bool is_can_go_to_index() const override;
        int go_to_index(int index) override;
        int go_forward() override;
        int go_back() override;
        bool is_can_go_back() const override;
};

template <typename T> class SequenceWriteStream : public WriteOnlyStream<T> {
    private:
        Sequence<T> *target;
        int position;
        bool opened;

        void ensure_opened() const;
    
    public:
        SequenceWriteStream(Sequence<T> &target);

        void open() override;
        void close() override;
        int write(const T &item) override;
        int get_position() const override;
        bool is_can_go_to_index() const override;
        int go_to_index(int index) override;
        int go_forward() override;
        int go_back() override;
        bool is_can_go_back() const override;
};

template <typename T> class FileReadStream : public ReadOnlyStream<T> {
    private:
        std::string path;
        T (*deserializer)(const std::string &item);
        mutable std::ifstream input;
        int position;
        bool opened;

        void ensure_opened() const;
    
    public:
        FileReadStream(const std::string &path, T (*deserialize)(const std::string &text));

        void open() override;
        void close() override;
        bool is_end_of_stream() const override;
        T read() override;
        int get_position() const override;
        bool is_can_go_to_index() const override;
        int go_to_index(int index) override;
        int go_forward() override;
        int go_back() override;
        bool is_can_go_back() const override;
};

template <typename T> class FileWriteStream : public WriteOnlyStream<T> {
    private:
        std::string path;
        std::string (*serializer)(const T &item);
        std::ofstream output;
        int position;
        bool opened;

        void ensure_opened() const;

    public:
        FileWriteStream(const std::string &path, std::string (*serializer)(const T &item));

        void open() override;
        void close() override;
        int write(const T &item) override;
        int get_position() const override;
        bool is_can_go_to_index() const override;
        int go_to_index(int index) override;
        int go_forward() override;
        int go_back() override;
        bool is_can_go_back() const override;
};

#include "Stream.tpp"
