# Instalador do jogo "Onde Fica?" (Projeto Integrador)
# Baixa o projeto do GitHub para a pasta Onde-Fica na area de trabalho.
#
# Uso (no PowerShell):
#   irm https://raw.githubusercontent.com/PQueirozDev/PAC-PI/main/docs/instalar.ps1 | iex
#
# Se o Git estiver instalado, o projeto e clonado (e atualizado com git pull
# nas proximas vezes). Sem o Git, o script baixa o .zip do GitHub e extrai.

& {
    $ErrorActionPreference = 'Stop'
    # o PowerShell 5 do Windows precisa disso para falar com o GitHub
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12

    $repositorio = 'https://github.com/PQueirozDev/PAC-PI'
    $areaDeTrabalho = [Environment]::GetFolderPath('Desktop')
    if ($env:ONDE_FICA_PASTA) {
        $destino = $env:ONDE_FICA_PASTA
    }
    else {
        $destino = Join-Path $areaDeTrabalho 'Onde-Fica'
    }

    Write-Host ''
    Write-Host '=== Onde Fica? - instalador ===' -ForegroundColor Yellow
    Write-Host "Pasta de destino: $destino"
    Write-Host ''

    $temGit = [bool](Get-Command git -ErrorAction SilentlyContinue)

    if (Test-Path $destino) {
        if ($temGit -and (Test-Path (Join-Path $destino '.git'))) {
            Write-Host 'A pasta ja existe. Atualizando com git pull...' -ForegroundColor Cyan
            git -C $destino pull --ff-only
            if ($LASTEXITCODE -ne 0) {
                Write-Host 'Nao consegui atualizar. Veja a mensagem do git acima.' -ForegroundColor Red
                return
            }
        }
        else {
            Write-Host 'A pasta ja existe e nao veio do git.' -ForegroundColor Red
            Write-Host 'Apague ou renomeie essa pasta e rode o comando de novo.'
            return
        }
    }
    elseif ($temGit) {
        Write-Host 'Clonando o repositorio com o git...' -ForegroundColor Cyan
        git clone "$repositorio.git" $destino
        if ($LASTEXITCODE -ne 0) {
            Write-Host 'O git clone falhou. Veja a mensagem acima.' -ForegroundColor Red
            return
        }
    }
    else {
        Write-Host 'Git nao encontrado. Baixando o .zip do GitHub...' -ForegroundColor Cyan
        $zip = Join-Path $env:TEMP 'onde-fica.zip'
        $extraido = Join-Path $env:TEMP 'onde-fica-extraido'
        if (Test-Path $extraido) { Remove-Item $extraido -Recurse -Force }
        Invoke-WebRequest "$repositorio/archive/refs/heads/main.zip" -OutFile $zip -UseBasicParsing
        Expand-Archive $zip -DestinationPath $extraido -Force
        # o zip do GitHub tem uma pasta dentro (PAC-PI-main); ela vira a pasta Onde-Fica
        $pastaDoZip = Get-ChildItem $extraido -Directory | Select-Object -First 1
        Move-Item $pastaDoZip.FullName $destino
        Remove-Item $zip, $extraido -Recurse -Force
    }

    Write-Host ''
    Write-Host 'Pronto! O projeto esta em:' -ForegroundColor Green
    Write-Host "  $destino"
    Write-Host ''
    Write-Host 'Para jogar: abra JogoAllegro.sln no Visual Studio 2022 e aperte F5.'
    Write-Host 'O Visual Studio baixa o Allegro sozinho pelo NuGet na primeira vez.'
    Write-Host ''

    # abre a pasta no Explorador de Arquivos
    Start-Process explorer.exe $destino
}
