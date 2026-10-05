/*
  Pikafish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2026 The Pikafish developers (see AUTHORS file)

  Pikafish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Pikafish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENGINE_FATAL_H_INCLUDED
#define ENGINE_FATAL_H_INCLUDED

#include <stdexcept>
#include <string>

namespace Pikafish {

/// 「引擎自己出事了」—— 宿主进程必须活下来，所以**不许 `exit()`**。
///
/// 为什么这不是洁癖：这个引擎在桌面端是**独立进程**（`uci_process_engine.dart` spawn 出来的），
/// 死掉只影响它自己；但在移动端它是**同一进程里的一个线程**（见 `pikafish_ffi.cpp` 的
/// `pikafish_main`），于是引擎里任何一处 `std::exit()` 都会把整个 App 带走。
///
/// 实测代价（iPhone 15 Plus / iOS 27，2026-09-23，`docs/architecture-plan.md` §11.33）：
/// 读屏 + 引擎分析跑着的时候，App 在 09:16:17 打印 `CoreAnalytics: Entering exit handler`
/// 正常退场 —— **没有崩溃报告、没有 jetsam 记录、没有信号**，用户看到的只是「助手突然没了」，
/// 排查方向上先怀疑了画中画、再怀疑了内存，最后才落到这里。上游那几处 `exit(EXIT_FAILURE)`
/// 本意是「独立进程里报错退出」，嵌进 App 之后语义变成了「连宿主一起杀」。
///
/// 所以致命路径统一抛这个异常，由 FFI 边界（`pikafish_ffi.cpp`）接住：引擎线程干净收工、
/// 把原因打一行可见日志，App 继续跑。
struct EngineFatalError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

}  // namespace Pikafish

#endif  // #ifndef ENGINE_FATAL_H_INCLUDED
