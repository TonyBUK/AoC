#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>
#include <assert.h>

class Registers
{
    public:

        enum RegisterType
        {
            E_REGISTER_FIRST = 0,
            E_REGISTER_A     = 0,
            E_REGISTER_B,
            E_REGISTER_LAST
        };

                        Registers(const int64_t a = 0, const int64_t b = 0);
        int64_t         Get      (const RegisterType eRegister) const;
        void            Set      (const RegisterType eRegister, const int64_t nValue);

        RegisterType    Decode   (const std::string& kRegister) const;

    private:

        int64_t m_nRegisterA;
        int64_t m_nRegisterB;
};

Registers::Registers(const int64_t a, const int64_t b) :
    m_nRegisterA    (a),
    m_nRegisterB    (b)
{
}

int64_t Registers::Get(const RegisterType eRegister) const
{
    switch (eRegister)
    {
        case E_REGISTER_A:
        {
            return m_nRegisterA;
        }

        case E_REGISTER_B:
        {
            return m_nRegisterB;
        }

        default:
        {
            assert(false);
        }
    }
}

void Registers::Set(const RegisterType eRegister, const int64_t nValue)
{
    switch (eRegister)
    {
        case E_REGISTER_A:
        {
            m_nRegisterA = nValue;
        } break;

        case E_REGISTER_B:
        {
            m_nRegisterB = nValue;
        } break;

        default:
        {
            assert(false);
        }
    }
}

Registers::RegisterType Registers::Decode(const std::string& kRegister) const
{
    if ("a" == kRegister)
    {
        return Registers::E_REGISTER_A;
    }
    else if ("b" == kRegister)
    {
        return Registers::E_REGISTER_B;
    }
    assert(false);
}

class Operand
{
    public:

                                Operand         (const Registers& kRegisters, const Registers::RegisterType eRegister);
                                Operand         (const Registers& kRegisters, const int64_t nLiteral);
        int64_t                 Get             () const;
        Registers::RegisterType Register        () const;

    private:

        const Registers&        m_kRegisters;
        bool                    m_bIsLiteral;
        Registers::RegisterType m_eRegister;
        int64_t                 m_nLiteral;
};

Operand::Operand(const Registers& kRegisters, const Registers::RegisterType eRegister) :
    m_kRegisters    (kRegisters),
    m_bIsLiteral    (false),
    m_eRegister     (eRegister),
    m_nLiteral      (0)
{
}

Operand::Operand(const Registers& kRegisters, const int64_t nLiteral) :
    m_kRegisters    (kRegisters),
    m_bIsLiteral    (true),
    m_eRegister     (Registers::E_REGISTER_A),
    m_nLiteral      (nLiteral)
{
}

int64_t Operand::Get() const
{
    if (m_bIsLiteral)
    {
        return m_nLiteral;
    }
    else
    {
        return m_kRegisters.Get(m_eRegister);
    }
}

Registers::RegisterType Operand::Register() const
{
    assert(!m_bIsLiteral);
    return m_eRegister;
}

class VirtualMachine
{
    public:

        VirtualMachine  ();

        void                Add     (const std::vector<std::string>& kRaw);
        void                Run     (const Registers& kRegisters);
        const Registers&    Get     () const;

    private:

        enum OpcodeType
        {
            E_OPCODE_HLF,
            E_OPCODE_TPL,
            E_OPCODE_INC,
            E_OPCODE_JMP,
            E_OPCODE_JIE,
            E_OPCODE_JIO
        };

        struct ByteCodeType
        {
            OpcodeType              eOpcode;
            std::vector<Operand>    kOperands;
        };

        void    AddSingleOperandRegister    (const OpcodeType eOpcode, const std::vector<std::string>& kOperands);
        void    AddSingleOperandRegister    (const OpcodeType eOpcode, const std::string&              kRegister);
        void    AddSingleOperandRegister    (const OpcodeType eOpcode, const Registers::RegisterType   eRegister);

        void    AddSingleOperandLiteral     (const OpcodeType eOpcode, const std::vector<std::string>& kOperands);
        void    AddSingleOperandLiteral     (const OpcodeType eOpcode, const std::string&              kLiteral);
        void    AddSingleOperandLiteral     (const OpcodeType eOpcode, const int64_t                   nLiteral);

        void    AddDoubleOperand            (const OpcodeType eOpcode, const std::vector<std::string>& kOperands);
        void    AddDoubleOperand            (const OpcodeType eOpcode, const std::string&              kRegister, const std::string& kLiteral);
        void    AddDoubleOperand            (const OpcodeType eOpcode, const Registers::RegisterType   eRegister, const int64_t      nLiteral);

        Registers                   m_kRegisters;
        std::vector<ByteCodeType>   m_kByteCode;

};

VirtualMachine::VirtualMachine() :
    m_kByteCode     (),
    m_kRegisters    ()
{
}

void VirtualMachine::Add(const std::vector<std::string>& kRaw)
{
    assert(kRaw.size() >= 2);

    if ("hlf" == kRaw.at(0))
    {
        std::vector<std::string> kOperands(kRaw.cbegin() + 1, kRaw.cend());
        AddSingleOperandRegister(E_OPCODE_HLF, kOperands);
    }
    else if ("tpl" == kRaw.at(0))
    {
        std::vector<std::string> kOperands(kRaw.cbegin() + 1, kRaw.cend());
        AddSingleOperandRegister(E_OPCODE_TPL, kOperands);
    }
    else if ("inc" == kRaw.at(0))
    {
        std::vector<std::string> kOperands(kRaw.cbegin() + 1, kRaw.cend());
        AddSingleOperandRegister(E_OPCODE_INC, kOperands);
    }
    else if ("jmp" == kRaw.at(0))
    {
        std::vector<std::string> kOperands(kRaw.cbegin() + 1, kRaw.cend());
        AddSingleOperandLiteral(E_OPCODE_JMP, kOperands);
    }
    else if ("jie" == kRaw.at(0))
    {
        std::vector<std::string> kOperands(kRaw.cbegin() + 1, kRaw.cend());
        AddDoubleOperand(E_OPCODE_JIE, kOperands);
    }
    else if ("jio" == kRaw.at(0))
    {
        std::vector<std::string> kOperands(kRaw.cbegin() + 1, kRaw.cend());
        AddDoubleOperand(E_OPCODE_JIO, kOperands);
    }
    else
    {
        assert(false);
    }
}

void VirtualMachine::AddSingleOperandRegister(const OpcodeType eOpcode, const std::vector<std::string>& kOperands)
{
    assert(kOperands.size() == 1);
    AddSingleOperandRegister(eOpcode, kOperands.at(0));
}

void VirtualMachine::AddSingleOperandRegister(const OpcodeType eOpcode, const std::string& kRegister)
{
    AddSingleOperandRegister(eOpcode, m_kRegisters.Decode(kRegister));
}

void VirtualMachine::AddSingleOperandRegister(const OpcodeType eOpcode, const Registers::RegisterType eRegister)
{
    ByteCodeType kEntry;
    kEntry.eOpcode  = eOpcode;
    kEntry.kOperands.push_back(Operand(m_kRegisters, eRegister));

    m_kByteCode.push_back(kEntry);
}

void VirtualMachine::AddSingleOperandLiteral(const OpcodeType eOpcode, const std::vector<std::string>& kOperands)
{
    assert(kOperands.size() == 1);
    AddSingleOperandLiteral(eOpcode, kOperands.at(0));
}

void VirtualMachine::AddSingleOperandLiteral(const OpcodeType eOpcode, const std::string& kLiteral)
{
    AddSingleOperandLiteral(eOpcode, std::stoll(kLiteral));
}

void VirtualMachine::AddSingleOperandLiteral(const OpcodeType eOpcode, const int64_t nLiteral)
{
    ByteCodeType kEntry;
    kEntry.eOpcode  = eOpcode;
    kEntry.kOperands.push_back(Operand(m_kRegisters, nLiteral));

    m_kByteCode.push_back(kEntry);
}

void VirtualMachine::AddDoubleOperand(const OpcodeType eOpcode, const std::vector<std::string>& kOperands)
{
    assert(kOperands.size() == 2);
    AddDoubleOperand(eOpcode, kOperands.at(0), kOperands.at(1));
}

void VirtualMachine::AddDoubleOperand(const OpcodeType eOpcode, const std::string& kRegister, const std::string& kLiteral)
{
    AddDoubleOperand(eOpcode, m_kRegisters.Decode(kRegister), std::stoll(kLiteral));
}

void VirtualMachine::AddDoubleOperand(const OpcodeType eOpcode, const Registers::RegisterType eRegister, const int64_t nLiteral)
{
    ByteCodeType kEntry;
    kEntry.eOpcode  = eOpcode;
    kEntry.kOperands.push_back(Operand(m_kRegisters, eRegister));
    kEntry.kOperands.push_back(Operand(m_kRegisters, nLiteral));

    m_kByteCode.push_back(kEntry);
}

void VirtualMachine::Run(const Registers& kRegisters)
{
    for (std::size_t nRegister = Registers::E_REGISTER_FIRST; nRegister < Registers::E_REGISTER_LAST; ++nRegister)
    {
        Registers::RegisterType eRegister = static_cast<Registers::RegisterType>(nRegister);
        m_kRegisters.Set(eRegister, kRegisters.Get(eRegister));
    }

    std::size_t nAddress = 0;

    while (nAddress < m_kByteCode.size())
    {
        const ByteCodeType& kOperation = m_kByteCode.at(nAddress);

        switch(kOperation.eOpcode)
        {
            case E_OPCODE_HLF:
            {
                const Operand& kOperand = kOperation.kOperands.at(0);
                m_kRegisters.Set(kOperand.Register(), kOperand.Get() / 2);
                ++nAddress;
            } break;

            case E_OPCODE_TPL:
            {
                const Operand& kOperand = kOperation.kOperands.at(0);
                m_kRegisters.Set(kOperand.Register(), kOperand.Get() * 3);
                ++nAddress;
            } break;

            case E_OPCODE_INC:
            {
                const Operand& kOperand = kOperation.kOperands.at(0);
                m_kRegisters.Set(kOperand.Register(), kOperand.Get() + 1);
                ++nAddress;
            } break;

            case E_OPCODE_JMP:
            {
                nAddress += kOperation.kOperands.at(0).Get();
            } break;

            case E_OPCODE_JIE:
            {
                if (0 == (kOperation.kOperands.at(0).Get() % 2))
                {
                    nAddress += kOperation.kOperands.at(1).Get();
                }
                else
                {
                    ++nAddress;
                }
            } break;

            case E_OPCODE_JIO:
            {
                if (1 == kOperation.kOperands.at(0).Get())
                {
                    nAddress += kOperation.kOperands.at(1).Get();
                }
                else
                {
                    ++nAddress;
                }
            } break;

            default:
            {
                assert(false);
            }
        }
    }
}

const Registers& VirtualMachine::Get() const
{
    return m_kRegisters;
}

std::vector<std::string> split(const std::string& s, const std::string& seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        pos     += seperator.length();
        prev_pos = pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        VirtualMachine  kVirtualMachine;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            std::size_t nCommaPos;
            do
            {
                nCommaPos = kLine.find(",");
                if (nCommaPos != std::string::npos)
                {
                    kLine.erase(nCommaPos, 1);
                }
            } while (nCommaPos != std::string::npos);

            const std::vector<std::string> kSplit = split(kLine, " ");
            kVirtualMachine.Add(kSplit);
        }

        kVirtualMachine.Run(Registers(0,0));
        std::cout << "Part 1: " << kVirtualMachine.Get().Get(Registers::E_REGISTER_B) << std::endl;

        kVirtualMachine.Run(Registers(1,0));
        std::cout << "Part 2: " << kVirtualMachine.Get().Get(Registers::E_REGISTER_B) << std::endl;
   }

    return 0;
}
